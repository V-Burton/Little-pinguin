#!/bin/bash

# Configuration
LOG_FILE="assignment_07_proof.txt"
DEBUGFS_PATH="/sys/kernel/debug/fortytwo"

# Initialisation du fichier de log
echo "==================================================" > $LOG_FILE
echo "   RAPPORT DE PREUVES - ASSIGNMENT 07 (DebugFS)" >> $LOG_FILE
echo "   Généré le : $(date)" >> $LOG_FILE
echo "   Utilisateur : $USER" >> $LOG_FILE
echo "==================================================" >> $LOG_FILE

# 1. Préparation du module
echo -e "\n[1] Préparation du module..."
sudo rmmod ft_debugfs_module.ko 2>/dev/null # Nettoyage au cas où
sudo insmod ft_debugfs_module.ko
if [ $? -eq 0 ]; then
    echo "SUCCÈS : Module ft_debugfs inséré." >> $LOG_FILE
else
    echo "ERREUR : Impossible d'insérer le module." >> $LOG_FILE
    exit 1
fi

# Application des droits sur le dossier parent (Requirement sujet)
sudo chmod 755 /sys/kernel/debug
sudo chown $USER $DEBUGFS_PATH
echo "SUCCÈS : Permissions appliquées sur $DEBUGFS_PATH." >> $LOG_FILE

# 2. Vérification visuelle des permissions (stat)
echo -e "\n[2] Vérification des modes et propriétaires..." >> $LOG_FILE
stat -c "Fichier: %n | Mode: %a | Owner: %U" $DEBUGFS_PATH/* >> $LOG_FILE

# 3. Test de 'id' (Attendu : RW pour tous)
echo -e "\n[3] TEST : Fichier 'id' (Permissions attendues: 0666)" >> $LOG_FILE
echo "--- Tentative d'écriture (vburton) ---" >> $LOG_FILE
echo "vburton_id" > $DEBUGFS_PATH/id 2>&1 >> $LOG_FILE
echo "--- Lecture du résultat ---" >> $LOG_FILE
cat $DEBUGFS_PATH/id >> $LOG_FILE 2>&1

# 4. Test de 'jiffies' (Attendu : RO pour tous)
echo -e "\n[4] TEST : Fichier 'jiffies' (Permissions attendues: 0444)" >> $LOG_FILE
echo "--- Lecture 1 ---" >> $LOG_FILE
cat $DEBUGFS_PATH/jiffies >> $LOG_FILE
sleep 1
echo "--- Lecture 2 (après 1s) ---" >> $LOG_FILE
cat $DEBUGFS_PATH/jiffies >> $LOG_FILE
echo "--- Tentative d'écriture (doit échouer) ---" >> $LOG_FILE
echo "12345" > $DEBUGFS_PATH/jiffies 2>&1 >> $LOG_FILE

# 5. Test de 'foo' (Attendu : Root Write, All Read)
echo -e "\n[5] TEST : Fichier 'foo' (Permissions attendues: 0644)" >> $LOG_FILE
echo "--- Tentative d'écriture par $USER (doit échouer) ---" >> $LOG_FILE
echo "Ceci est un test" > $DEBUGFS_PATH/foo 2>&1 >> $LOG_FILE

if [ $? -ne 0 ]; then
    echo "RÉSULTAT : Échec d'écriture confirmé (Permission Denied)." >> $LOG_FILE
fi

echo "--- Tentative d'écriture par ROOT (doit réussir) ---" >> $LOG_FILE
sudo sh -c "echo 'Donnees_Securisees_Root' > $DEBUGFS_PATH/foo" 2>&1 >> $LOG_FILE
echo "RÉSULTAT : Éécriture root réussie." >> $LOG_FILE

echo "--- Lecture par $USER (doit réussir) ---" >> $LOG_FILE
cat $DEBUGFS_PATH/foo >> $LOG_FILE 2>&1

# 6. Test de persistance et Mutex (Append)
echo -e "\n[6] TEST : Persistance et Append sur 'foo'" >> $LOG_FILE
sudo sh -c "echo ' + Suite_de_donnees' >> $DEBUGFS_PATH/foo"
echo "Contenu final de foo :" >> $LOG_FILE
cat $DEBUGFS_PATH/foo >> $LOG_FILE

# 7. Déchargement et nettoyage
echo -e "\n[7] Nettoyage et sortie..." >> $LOG_FILE
sudo rmmod ft_debugfs_module.ko
if [ ! -d "$DEBUGFS_PATH" ]; then
    echo "SUCCÈS : Le répertoire $DEBUGFS_PATH a été supprimé." >> $LOG_FILE
fi

echo -e "\n--- FIN DU RAPPORT ---" >> $LOG_FILE

echo "Terminé ! Le fichier '$LOG_FILE' a été généré."