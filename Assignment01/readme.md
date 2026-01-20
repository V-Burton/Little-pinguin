# Spécification makefile
Explication commande: `make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules` 

- Il va dans `/lib/modules/<kernel-version>/build`
- Charge le makefile principal du noyau.
- Puis entre dans mon dossier courant avec le `M=$(PWD)` et compile les fichiers listés dans mon Makefile.
- la clé `module` à la fin spécifie que on compile un module.


### Chargement d'un module simple
`insmod <mon_module>`

### Suppression d'un module simple
`rmmod <mon_module>`