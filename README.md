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
- 1 Résistance 100Ω
- 2 Boutons poussoir (`control`, `reset`)
- 2 Capteurs de fin de course (`sw_open`, `sw_close`)
- 1 Keypad

## Diagramme d'état-transition

Le fonctionnement de la porte est régi par un **diagramme d'état-transitions** (fourni en image `.png` dans le livrable).  
*A retrouver dans le dossier `files/`*

## Structure du Projet

- **PlatformIO** : gestion du code C++ pour microcontrôleur
- **Wokwi** : simulation du câblage
- **Diagramme** : fichier image `.png` du diagramme d’état

## Bonus : Mot de passe de déverrouillage

- La saisie d'un mot de passe est attendu lors du verrouillage de la porte.  
  **Feedback visuel** via les LED :
  - 🔴 **Rouge continue** : attente de la saisie du mot de passe
  - 🟠 **Orange clignotant** : prise en compte de l'input
  - 🟢 **Vert clignotant** : mot de passe enregisté (appuyer sur `*` pour validé)
  - 🔴 **Rouge clignotant** : porte verrouillée

- Le mot de passe est requise pour déverrouiller de la porte.  
  **Feedback visuel** via les LED :
  - 🔴 **Rouge continue** : attente de la saisie du mot de passe
  - 🟠 **Orange clignotant** : prise en compte de l'input
  - 🟢 **Vert clignotant** : mot de passe correcte (appuyer sur `*` pour validé). *Retour a l'état FERMÉ 🔴 **Rouge continue***
  - 🔴 **Rouge clignotant** : mot de passe incorrect, porte verrouillée

#### Retrouvez la version de base sur la branche `default`