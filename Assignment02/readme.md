# Procédure de modification de l'EXTRAVERSION et création de patch

### 1. Modification du Makefile
- Édition du fichier racine `Makefile`.
- Modification de la variable : `EXTRAVERSION = -thor_kernel`.

### 2. Validation et Compilation
- Compilation du noyau : `make -j$(nproc)`.
- Installation et mise à jour de GRUB.
- Reboot et vérification avec `uname -r` (doit afficher `-thor_kernel`).
- Extraction du log : `dmesg > boot_thor.log`.

### 3. Génération du Patch (Standards Linux)
- Création d'un commit signé :
  `git add Makefile`
  `git commit -s"`
  - Remplir le git commit avec une description
- Génération du fichier patch :
  `git format-patch HEAD~1`

### 4. Vérification du livrable
- Test de conformité du patch :
  `./scripts/checkpatch.pl 0001-kbuil-change-EXTRAVERSION-to-thor_kernel.patch `

### 5. Note sur les linux submission standard
- En tant normal la modification de EXTRAVERSION au profit d'un nom de kernel custom n'est pas permisse et devrait être faite dans la variable `CONFIG_LOCALVERSION` ou `LOCALVERSION`

### 6. Check validité du patch
- La norme du patch peut être vérifié à l'aide de cette commande qui utilise un script du kernel pour verifier spécifiquement l'integrité d'un patch.

### 5.Execution du nouveau noyau
- Similaire au readme de l'assignment00