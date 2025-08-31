# Feather-Weather
## Histoire
Feather-Weather est le nom que j'ai choisi de donner à mon travail de maturité sur l'électronique. Originalement, ce projet était destiné à l'enseignement, comme support solide pour un cour ou tout autre activité pédagogique. Après quelques mois de travail et la réalisation des différents prototypes, il s'est avéré que mon travail avait pris une autre tournure que celle imaginée au commencement. Il était devenu difficile de faire apparaître les éléments qui le rendaient "facile d'accès" car ils avaient basculer sur le plan secondaire pour laisser place à un code un peu plus ajsuté et surtout respectant quelques principes de programmation. Ce travail a donc migré vers un public cible un peu plus averti, possédant déjà des notions en porgramation sur les systèmes embarqués. Il est donc aujourd'hui mis à disposition de tous, dans le seul but de lui rendre sa réelle utilité.

## Compilation & Usage
### 1. Cloner le repo git
```bash
git clone https://github.com/stackOverHeap/feather-weather.git
```
### 2. Compiler avec platformIO
```bash
pio run
```
### 3. Flasher sur un micro <br />
flash du code s'éxecutant sur la station interne
```bash
pio run --target upload --environment feather32u4_ISTA
```
flash du code s'éxecutant sur la station externe
```bash
pio run --target upload --environment feather32u4_ESTA
```
### 4. Verifier la connexion entre les dispositifs
Pour ceci, s'assurer que l'icone de connexion n'apparaît pas sur l'interface de l'appareil. Un certain temps peut être nécessaire avant la détection du signal.
