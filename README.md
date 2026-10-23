# Rasfloppy

Faire jouer de la musique à un lecteur de disquette avec un Raspberry Pi.

## Fichiers

| Fichier | Rôle |
|---|---|
| `main.c` | La mélodie (suite de `play_note` et `delay`) |
| `floppy_driver.c / .h` | Pilotage de la tête (broches STEP et DIR) |
| `timer.h / timer.o` | Timer fourni (compilé pour ARM) |
| `Makefile` | Compilation |
| `sim/` | Simulation sur PC (sans Raspberry Pi) |

## Sur le Raspberry Pi (avec le vrai lecteur)

```bash
make
sudo ./rasfloppy
```

- `Ctrl+C` pour arrêter.
- Si `make` échoue : vérifiez que WiringPi est installé et que `timer.o` est présent.
- Pour tout supprimer et recompiler : `make clean && make`
  (`timer.o` n'est jamais supprimé).

## Sur PC (simulation, sans Raspberry Pi)

Depuis le dossier du projet :

```bash
make sim
timeout -s INT 10 ./rasfloppy_sim
python3 sim/analyse.py steps.log
```

1. `make sim` compile le projet avec un faux WiringPi.
2. `timeout -s INT 10 ./rasfloppy_sim` lance le programme pendant 10 secondes (possibilité de mofifier la durée en fonction de la musique), puis écrit `steps.log` (changez `10` selon la durée de la mélodie).
3. `python3 sim/analyse.py steps.log` affiche les notes jouées et la position
   de la tête, et crée `sortie.wav` (à écouter).

À chaque modification de `main.c`, refaire les 3 commandes.

> N'utilisez **pas** `make` tout seul sur PC : il cherche le vrai WiringPi,
> qui n'existe que sur le Raspberry Pi.
