Ce projet consiste à concevoir un module noyau Linux capable de s'auto-charger dès la détection d'un clavier USB. L'objectif est de maîtriser les mécanismes de **Hotplug**, de **Modalias** et l'interaction entre le noyau et l'espace utilisateur.

---

## 🧠 Concepts Théoriques

### 1. Le mécanisme de Modalias
Le noyau Linux n'utilise pas le nom des fichiers pour faire correspondre un matériel à un driver. Il utilise des **Modalias**.
* Chaque périphérique USB possède une "carte d'identité" basée sur sa classe, son vendeur et son protocole.
* Lors du branchement, le noyau génère une chaîne de caractères (ex: `usb:v*p*ic03isc01ip01*`).
* Les outils d'espace utilisateur (`udev`) comparent cette chaîne avec une table de correspondance générée par `depmod`.

### 2. La chaîne de détection (Workflow)
1. **Hardware** : Branchement du clavier (via QEMU `device_add`).
2. **Kernel** : Détection du bus USB -> Génération d'un **Uevent**.
3. **Udev** : Reçoit l'événement et cherche le module correspondant dans `/lib/modules/$(uname -r)/modules.alias`.
4. **Kmod** : Charge le module via la commande `modprobe`.



---

## 🛠 Structure du Module

Le module repose sur la macro `MODULE_DEVICE_TABLE`. Pour cibler **tous** les claviers sans distinction de marque, nous utilisons les descripteurs d'interface standard :

| Champ | Valeur (Hex) | Description |
| :--- | :--- | :--- |
| **Interface Class** | `03` | Classe HID (Human Interface Device) |
| **Interface SubClass** | `01` | Protocole de Boot (BIOS) |
| **Interface Protocol** | `01` | Clavier (Keyboard) |

---

## 🚀 Étapes d'Installation et de Test

### Étape 1 : Compilation
Compilez le module sur votre environnement de développement LFS.
```bash
make
```
### Étape 2 : Installation Système
Pour que le mécanisme de Hotplug fonctionne, le binaire .ko doit être placé dans l'arborescence officielle.

#### 1. Création du dossier de destination (convention LFS/Ubuntu)
sudo mkdir -p /lib/modules/$(uname -r)/extra

#### 2. Copie du module
sudo cp main.ko /lib/modules/$(uname -r)/extra/

#### 3. Mise à jour de la base de données des alias
```bash 
sudo depmod -a
```

### Etape 3: Tester en branchant différent devices
Pour notre evaluation nous utilisont Qemu afin de simuler un branchement de différents périphérique.
```bash
qemu-system-x86_64 \
  -m 8G \
  -smp 12 \
  -enable-kvm \
  -cpu host \
  -drive file="/media/vburton/Extreme SSD/Little-pinguin/little-pinguin_qemu/little-pinguin_qemu.vdi",format=vdi,if=ide \
  -device qemu-xhci,id=usb \
  -device usb-tablet \
  -device usb-kbd,id=mon_clavier \
  -device usb-mouse,id=ma_souris \
  -vga virtio \
  -nic user,model=e1000,hostfwd=tcp::9999-:22
```
Dans le moniteur QEMU (Ctrl+Alt+2), simulez le branchement :
```Bash
(qemu) device_add usb-kbd,id=test_kbd
```
Vérifiez le chargement automatique dans la console SSH :
```Bash
lsmod | grep main
dmesg | tail -n 20
```
---
### 🔍 Analyse des outils utilisés
**MODULE_DEVICE_TABLE** : Macro C exportant les IDs du module pour depmod.

**depmod** : Utilitaire analysant les modules pour créer modules.alias.

**udev** : Démon qui écoute les événements du noyau et déclenche le chargement.

**kmod / modprobe** : Outils de bas niveau pour insérer le module en gérant les dépendances.
