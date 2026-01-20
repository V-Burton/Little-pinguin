# Procédure de Compilation et Migration du Noyau Linux

### 1. Préparation de l'environnement
```bash
cd /sources
tar -xvf linux-master.tar.gz
cd linux-master
make mrproper                   # Nettoyage complet de l'arbre des sources
```
### 2.Configuration (Migration de la config LFS)
```bash 
cp ../linux-6.16.1/.config .    # Import de la configuration stable actuelle
make olddefconfig               # Mise à jour automatique des nouvelles options (6.19)
sed -i 's/# CONFIG_LOCALVERSION_AUTO is not set/CONFIG_LOCALVERSION_AUTO=y/' .config
grep "CONFIG_LOCALVERSION_AUTO" .config  # Validation de la consigne spécifique
```
### 3.Compilation
```bash
make -j$(nproc)                 # Compilation parallélisée du noyau et des modules
```

### 4.Installation manuelle
```bash
make modules_install            # Installation des pilotes dans /lib/modules/6.19.0-rc4
# Copie de l'image du noyau dans le dossier /boot
cp -iv arch/x86/boot/bzImage /boot/vmlinuz-<Nom de votre distrib>
```

### 5.Configuration de l'amorceur (GRUB)
```bash
grub-mkconfig -o /boot/grub/grub.cfg  # Mise à jour du menu de démarrage
grep "vmlinuz-torvalds-master" /boot/grub/grub.cfg # Vérification de l'entrée GRUB
```

### 6.Génération des livrables (Post-reboot)
```bash
uname -a                        # Vérification de la version active (6.19.0-rc4-g...)
dmesg > boot.log                # Extraction du journal de démarrage
cp .config kernel_config.txt    # Copie du fichier de configuration pour rendu
```