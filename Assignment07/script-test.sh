#!/bin/bash
# Script de validation Assignment 07

sudo insmod ft_debugfs_module.ko

# Requirement: Globally readable directory
sudo chmod 755 /sys/kernel/debug
sudo chown $USER /sys/kernel/debug/fortytwo

echo "--- 1. Checkd es droits ---"
ls -ld /sys/kernel/debug/fortytwo
ls -l /sys/kernel/debug/fortytwo

echo -e "\n--- 2. Test ID (All users RW) ---"
echo "vburton" > /sys/kernel/debug/fortytwo/id
cat /sys/kernel/debug/fortytwo/id
echo vburton > /sys/kernel/debug/fortytwo/id
echo coucou > /sys/kernel/debug/fortytwo/id

echo -e "\n--- 3. Test Jiffies (Read-only) ---"
cat /sys/kernel/debug/fortytwo/jiffies
sleep 1
cat /sys/kernel/debug/fortytwo/jiffies

echo -e "\n--- 4. Test FOO (Root write, All read, Mutex) ---"
echo "Contenu initial" > /sys/kernel/debug/fortytwo/foo
cat /sys/kernel/debug/fortytwo/foo
echo " Ajout via Append" >> /sys/kernel/debug/fortytwo/foo
cat /sys/kernel/debug/fortytwo/foo
echo "Reecriture" > /sys/kernel/debug/fortytwo/foo

echo -e "\n--- 5. Nettoyage ---"
sudo rmmod ft_debugfs_module.ko
ls /sys/kernel/debug/fortytwo 2>/dev/null || echo "Dossier bien supprimé."