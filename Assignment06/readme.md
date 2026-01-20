# 📑 Assignment 06: Compilation de Linux-Next
Ce projet consiste à récupérer, configurer et compiler le noyau Linux-Next, l'arbre de développement le plus à jour de Linux.

# 🧠 Le rôle de Linux-Next : Le "SAS de sécurité"
Avant d'atteindre le noyau officiel (Mainline), le code doit prouver sa stabilité. Linux-Next est l'endroit où l'on fusionne quotidiennement les travaux de centaines de mainteneurs pour s'assurer qu'ils fonctionnent ensemble.
En opposition à l'arbre "Mainline" de Linus Torvalds, qui est la version stable officielle.

## Pourquoi est-ce nécessaire ?
Dans un projet avec des milliers de contributeurs, deux développeurs peuvent modifier le même fichier sans le savoir. linux-next permet de fusionner tous les travaux en cours chaque jour pour vérifier :Les conflits de fusion (Git conflicts).Les erreurs de compilation croisées.Les régressions de performance.

# ⏳ Cycle Temporel des Améliorations
Le noyau suit un rythme cyclique de 9 à 10 semaines.
Le développement d'une version (ex: 6.13) dure environ 70 jours et se divise en trois étapes strictes :

### 1. La Phase d'Accumulation
- **Où** : Dans les arbres des mainteneurs (ex: net-next, usb-next) et Linux-Next.
- **Action** : Les développeurs soumettent leurs patchs. Le code est testé, critiqué et corrigé.
- **But** : Préparer les nouveautés en isolation.
Statut : Très instable.

### 2. La Merge Window (Le "Sprint" de 14 jours)
- **Où** : Passage de Linux-Next vers Mainline (Linus Torvalds).
- **Action** : Linus ouvre les vannes. Il fusionne les nouveautés validées durant la phase précédente.
- **Condition** : Linus n'accepte généralement que du code qui a déjà séjourné dans Linux-Next.
- **Résultat** : Publication de la version rc1 (Release Candidate 1).

### 3. La Phase de Stabilisation
- **Où** : Arbre Mainline.
- **Durée** : 7 à 8 semaines.
- **Action** : Interdiction d'ajouter des fonctionnalités. On ne fait que corriger des bugs (Bugfixes). Chaque dimanche, une nouvelle version de test sort (rc2, rc3... jusqu'à rc8).
- **Résultat** : Sortie de la Version Finale (Stable).
 
### Voyage dans le temps d'une amélioration :

| Phase |Durée | Action dans l'Arbre Git | État du Code |
|---|---|---|---|
Développement | Continu | Le patch est envoyé sur une Mailing List. |Brouillon / Review
Acceptation | Variable | Le mainteneur l'intègre dans son Subsystem Tree (ex: usb-next). | Validé localement
Intégration | Quotidien | Le code est fusionné dans Linux-Next. | Test d'intégration global
Merge Window | 2 semaines | Linus Torvalds fusionne Linux-Next dans Mainline. | Version RC1 (Release Candidate)
Stabilisation | 7-8 semaines | Corrections de bugs uniquement (rc2 -> rc8). | Stabilisation
Release | Jour J | La version finale sort (ex: 6.13). | Stable

## Note
Url ou trouver le kernel linux-next: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/