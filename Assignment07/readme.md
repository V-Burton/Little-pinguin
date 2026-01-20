# Assignement 07: debugfs

### monter debugf:
Checker si debugfs est bien monter avec `ls /sys/kernel/debug`.
Si il y a une sortie c'est bon sinon: `mount -t debugfs none /sys/kernel/debug`

## 📁 1. Qu'est-ce que DebugFS ?
**DebugFS** est un système de fichiers virtuel (en RAM) conçu spécifiquement pour le débogage. Contrairement à `/proc` (orienté processus) ou `/sys` (orienté périphériques/modèle objet), **DebugFS** n'a aucune règle stricte : il est là pour que le développeur puisse exposer ce qu'il veut.

### Montage et Vérification
La plupart des distributions modernes montent DebugFS automatiquement dans `/sys/kernel/debug`.
- Vérifier s'il est monté: `mount | grep debugfs`
- Le monter manuellement: `sudo mount -t debugfs none /sys/kernel/debug`



## 🛠 2. Architecture du Module
### Gestion de la Mémoire
- Buffer Statique : Utilisation d'une page mémoire (**PAGE_SIZE**, généralement 4096 octets) pour stocker les données de manière persistante sans allocation dynamique répétée (kmalloc).
- Sémantique UNIX : Implémentation du support complet pour l'écrasement (>) via `O_TRUNC` et l'ajout (>>) via `O_APPEND`.

### Concurrence
Pour éviter les Race Conditions (concurrence entre deux processus écrivant en même temps), nous utilisons un Mutex.
- `mutex_lock_interruptible()` : Verrouille l'accès. Le mode "interruptible" permet de ne pas bloquer un processus dans un état "D" (uninterruptible sleep) si l'utilisateur fait un Ctrl+C.

## 📜 3. Les Bonnes Pratiques Noyau (Kernel Rules)
1. **Utilisation des Helpers VFS** : Privilégier `simple_read_from_buffer` et `simple_write_to_buffer`. Ils gèrent automatiquement la copie sécurisée entre l'espace noyau et utilisateur ainsi que la mise à jour des offsets (ppos).

2. **Vérification des Permissions** : Bien que nous définissions des modes (ex: 0644), c'est la couche VFS qui valide les droits avant d'appeler nos fonctions.

3. **Gestion des Inodes** : Utiliser `i_size_write` pour informer le noyau de la taille "logique" de nos fichiers virtuels.

4. **Pas de Floating Point** : Le noyau ne gère pas les nombres à virgule (float/double).

5. **Déclarations C89** : Déclarer toutes les variables au début des fonctions avant le code exécutable.


### Accès aux fichiers
Par défaut, seul root peut lire `/sys/kernel/debug`. Pour respecter la consigne "Globally Readable" :
```bash
chmod 755 /sys/kernel/debug
```

## 🔍 4. Analyse des Callbacks (file_operations) de foo
- `.open` : Intercepte les flags O_TRUNC pour réinitialiser la taille et O_APPEND pour placer le curseur à la fin.

- `.read` : Retourne impérativement le nombre d'octets lus. Retourner 0 est le seul moyen de signaler la fin du fichier (EOF) au programme cat.

- `.write` : Copie les données utilisateur vers le buffer noyau et met à jour l'index de fin (current_data_len).

- `.release` : Appelé lors de la fermeture du dernier descripteur de fichier. Utile pour le nettoyage de ressources.


## 🔍 5.Appronfondissement technique:
### 1. Inode
Le mot Inode (pour Index-Node) est la carte d'identité "physique" d'un fichier sur le système de fichiers.

Imaginer un livre dans une bibliothèque :

- Le Dentry (Directory Entry) : C'est le titre sur la tranche du livre et son emplacement sur l'étagère (le nom du fichier, ex: foo).

- L'Inode : C'est la fiche technique collée à l'intérieur du livre. Elle contient :
    - La taille du fichier (combien de pages).

    - Les permissions (qui peut le lire).

    - Les dates (création, modification).

    - Le propriétaire.

**Important** : L'inode ne connaît pas le nom du fichier. Il ne connaît que ses propriétés techniques et où se trouvent les données en mémoire (`my_buffer`).

#### Pourquoi doit-on le manipuler ici ?
---
Dans un système de fichiers classique (comme celui de notre disque dur), c'est le système de fichiers (EXT4, NTFS) qui met à jour l'inode automatiquement quand tu écris.

Mais dans notre module, nous somme le système de fichiers. DebugFS nous donne une structure vide, et c'est à nous de tenir la fiche technique à jour.

#### **Le problème de la taille (`i_size`)**
---
Si on n'écrit pas la taille dans l'inode via `i_size_write()`, pour Linux, notre fichier fait 0 octet.

Quand on fait `cat`, le système regarde l'inode : "Taille 0 ? Ok, je ne lis rien".

Quand on fait `>>` (Append), le système regarde l'inode : "Taille 0 ? Ok, je commence à écrire à l'offset 0".

C'est pour cela que nous avons ajouté : `i_size_write(inode, current_data_len);`. Cela synchronise notre variable logicielle (current_data_len) avec la réalité du système (inode->i_size).


#### Inode vs File : Ne pas confondre !
---
C'est l'erreur classique. En noyau, nous manipulons deux structures différentes dans nos fonctions :
Caractéristique | `struct inode` | `struct file`
---|---|---
Représente | Le fichier sur le disque/RAM (Statique) | Une instance d'ouverture (Dynamique)
Unicité | Il n'y a qu'un seul inode par fichier. | Il y a un struct file par open().
Contient | "Taille, Permissions, Propriétaire." | "Offset actuel (`f_pos`), Flags (`O_RDONLY`, `O_TRUNC`)."

**Analogie** : Si 3 personnes font cat foo en même temps, il y a 3 structures file mais toujours 1 seul inode.

#### Pourquoi le déclarer dans les arguments des fonctions ?
Dans tes fonctions `open` ou `release`, le noyau te passe l'inode en paramètre : `static int foo_open(struct inode *inode, struct file *file)`

Cela te permet de :    
1. Identifier quel fichier est ouvert (si tu utilises la même fonction pour plusieurs fichiers).
    
2. Modifier ses propriétés (comme nous l'avons fait pour la taille).

3. Accéder à des données privées liées au périphérique.

### 2. Dentry (Directory entry)

Le `struct dentry` est le pont entre le nom d'un fichier et son Inode.

Dans le noyau, un fichier ne "connaît" pas son nom. L'Inode contient les données et les permissions, mais c'est le dentry qui contient la chaîne de caractères (ex: "foo") et la hiérarchie (qui est le parent de qui).

#### A quoi ça sert concrètement ?
---
- La Hiérarchie (Le chemin) : Le dentry de foo contient un pointeur vers le dentry de son parent fortytwo, qui lui-même pointe vers debug. C'est grâce à cela que Linux peut résoudre un chemin comme /sys/kernel/debug/fortytwo/foo.

- Le Cache (Performance) : Le "Dentry Cache" (dcache) est crucial. Quand tu fais un ls, le noyau ne va pas fouiller le disque ou demander au driver de tout recalculer à chaque fois. Il garde les dentry en mémoire pour un accès instantané.

- Le Lien : Un dentry pointe vers un seul Inode. Mais (théoriquement) plusieurs dentry peuvent pointer vers le même Inode (c'est le principe des hard links).

#### Pourquoi ton code manipule des struct dentry * ?
---
Quand on appelle debugfs_create_file, la fonction fait deux choses en interne :

1. Elle crée un Inode pour stocker nos file_operations.

2. Elle crée un Dentry pour lier cet Inode au nom "foo" à l'intérieur du dossier parent my_dir.

Elle renvoie le pointeur vers le Dentry car c'est la poignée dont on a besoin pour supprimer le fichier plus tard.

