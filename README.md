# TD - Porte de Garage 🚪

## Description

Ce projet simule le contrôle d’une porte de garage automatisée à l’aide d’un bouton poussoir, de LED et de capteurs de position. Il inclut également une gestion du verrouillage de la porte et une détection d’erreur.

## Fonctionnalités

- **Ouverture/Fermeture de la porte** : Appui long (2 secondes) sur le bouton.
- **Verrouillage/Déverrouillage** : Appui très long (5 secondes) lorsque la porte est fermée.
- **Feedback visuel** via des LED :
  - 🔴 **Rouge** : porte fermée
  - 🔴 **Rouge clignotant** : porte verrouillée
  - 🟠 **Orange clignotant** : porte en mouvement (ouverture/fermeture)
  - 🟢 **Vert** : porte ouverte
  - 🔴🟠🟢 **Simultanés** : erreur (porte met plus de 5s pour s’ouvrir/fermer)

## Règles de comportement

- Le bouton est **inactif** durant l’ouverture/fermeture.
- La porte doit être **déverrouillée** pour pouvoir être ouverte/fermée.
- Si l’état de la porte est **inconnu** ou en **erreur**, elle est considérée comme dans un état *IDLE*, qui redirigera dans le bon état en fonction des condition *(differentes infos des capteurs)*.

## Matériel utilisé

- 3 LED (Rouge, Orange, Verte)
- 3 Résistances 100Ω
- 1 Bouton poussoir (control)
- 2 Capteurs de fin de course (`sw_open`, `sw_close`)

#### *Bonus :*
- *1 Bouton poussoir (reset, après l'état d'erreur)*
- *4 LED (Cyan, Bleu, Mauve, Jaune) (LED de débug)*
- *1 Résistances 100Ω*

## Diagramme d'état-transition

Le fonctionnement de la porte est régi par un **diagramme d'état-transitions** (fourni en image `.png` dans le livrable).  
*A retrouver dans le dossier `files/`*

## Structure du Projet

- **PlatformIO** : gestion du code C++ pour microcontrôleur
- **Wokwi** : simulation du câblage
- **Diagramme** : fichier image `.png` du diagramme d’état

