# Basic des coding style de Linux

### 1. Indentation et Espacement
Tabulations uniquement : L'indentation se fait avec des tabulations réelles de 8 caractères. Les espaces sont interdits pour l'indentation.

Longueur de ligne : Essayez de ne pas dépasser 80-100 caractères par ligne pour garantir la lisibilité sur tous les terminaux.

Pas d'espaces en fin de ligne : Supprimez les "trailing whitespaces".

### 2. Placement des Accolades
Le noyau utilise le style K&R (Kernighan & Ritchie) :

Fonctions : L'accolade ouvrante est sur la ligne suivante.
```c
int main(void) 
{
    ...
}
```

Blocs (if, switch, for, while) : L'accolade ouvrante est sur la même ligne que le mot-clé.

**Exceptions** : Si un bloc if ne contient qu'une seule ligne, on n'utilise pas d'accolades (sauf si un else en a besoin).
```c
if (x < 10) {
    print(x);
    x++
}

if (x > 10)
    print(x);
```

### 3. Conventions de Nommage
Snake_case : Utilisez `ma_variable` ou `ma_fonction`. Pas de CamelCase (maVariable).

Variables locales : Noms courts et descriptifs (ex: i pour un compteur, tmp).

Fonctions globales/exportées : Doivent être préfixées par le nom du module pour éviter les collisions (ex: thor_kernel_init).

### 4. Macros et Types
Macros : En majuscules (`MA_MACRO`). Si elle contient plusieurs lignes, utilisez le bloc `do { ... } while (0)`.

Types spécifiques : Utilisez les types du noyau pour la portabilité : u8, u16, u32, u64 (non signé) ou s8, s16... au lieu des types C standard.

### 5. Commentaires et Licence
Licence (SPDX) : La première ligne de chaque fichier doit indiquer la licence.

Format : Utilisez /* ... */ pour les commentaires. Pour les fonctions, utilisez le format KernelDoc (/** ... */) juste avant la définition.

### 6. Affichage des messages
Ne plus utiliser printk(KERN_INFO ...). Préférez les macros modernes qui incluent le contexte :

pr_info("Message\n"); (pour un module général)

dev_info(dev, "Message\n"); (si vous avez une structure device, c'est le standard absolu).

### 7. Check du respect des normes de code
- Idem que pour checker les patch en ajoutant le flag `--file`
```bash
perl scripts/checkpatch.pl --file /home/vburton/little-pinguin/Assignment03/sleep.c
```

