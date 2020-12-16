# Schwarz_DD_CHP_2020 ~ Lyra

Projet CHP / schwarz / parallèle

## Compilation

Pour compiler, merci de créer un répertoire **build** à la racine du projet. Appeler ensuite cmake dedans par `cmake ..` (quelques options doivent être activées dedans pour pouvoir utiliser Metis ou Scotch par exemple, regarder du côté de `cmake-gui ..`)

Pour compiler `make` (si Metis et/ou Scotch sont activés, cmake les téléchargera et les installera dans le dossier **build**)

## partition-lyra

L'exécutable **partition-lyra** nécéssite un maillage d'entrée au format **LYRA** (voir format lyra)
