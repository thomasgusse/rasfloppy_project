#!/usr/bin/env python3
"""Analyse steps.log produit par rasfloppy_sim : notes jouées, position de la tête, pas perdus."""
import math, struct, sys, wave

NB_TRACKS    = 80      # pistes d'un lecteur 3,5"
START_POS    = 40      # position inconnue de la tête au démarrage
MIN_STEP_US  = 1000    # en dessous, on suppose que le moteur perd le pas
NAMES = ["do","do#","ré","ré#","mi","fa","fa#","sol","sol#","la","la#","si"]

def note_name(f):
    n = round(69 + 12 * math.log2(f / 440.0))
    return f"{NAMES[n % 12]}{n // 12 - 1}"

raw, marks = [], {}
for line in open(sys.argv[1] if len(sys.argv) > 1 else "steps.log"):
    if line.startswith("#"):
        _, label, t = line.split()
        marks[label] = int(t)
    else:
        t, d = map(int, line.split())
        raw.append((t, d))
if not raw:
    sys.exit("aucun pas enregistré")

# --- modèle physique simple de la tête ---------------------------------
pos, last, lost = START_POS, None, 0
bump_stop = bump_far = 0
min_pos = max_pos = pos
accepted = []
for t, d in raw:
    if last is not None and t - last < MIN_STEP_US:
        lost += 1                      # pas trop rapproché : perdu
        continue
    last = t
    accepted.append((t, d))
    pos += 1 if d else -1              # DIR=HIGH : on s'éloigne de la butée
    playing = t >= marks.get("timer_start", raw[0][0])
    if pos < 0:
        pos = 0
        bump_stop += playing           # (le reset tape volontairement la butée)
    elif pos > NB_TRACKS - 1:
        pos = NB_TRACKS - 1
        bump_far += playing
    min_pos, max_pos = min(min_pos, pos), max(max_pos, pos)

t_play = marks.get("timer_start", raw[0][0])
reset_steps = [a for a in accepted if a[0] < t_play]
print("=== Tête de lecture ===")
print(f"pas émis : {len(raw)}  | perdus (< {MIN_STEP_US} µs d'écart) : {lost}")
if lost:
    print(f"  /!\\ {lost} pas trop rapides : le moteur ne suivra pas (typique d'un reset sans delay)")
p = START_POS
for t, d in reset_steps:
    p = max(0, min(NB_TRACKS - 1, p + (1 if d else -1)))
print(f"position (départ supposé {START_POS}) → après reset : {p} (0 = butée attendue)")
print(f"positions atteintes : {min_pos} .. {max_pos}")
print(f"chocs contre une butée PENDANT la lecture : côté 0 = {bump_stop} | côté opposé = {bump_far}")

# --- notes jouées -------------------------------------------------------
play = [t for t, _ in accepted if t >= t_play]
W = 20
labels = []
for i in range(len(play) - W):
    f = W / ((play[i + W] - play[i]) / 1e6)
    labels.append(round(69 + 12 * math.log2(f / 440.0)))
med = [sorted(labels[max(0, i - 3): i + 4])[len(labels[max(0, i - 3): i + 4]) // 2]
       for i in range(len(labels))]

print("\n=== Notes jouées ===")
i = 0
while i < len(med):
    j = i
    while j + 1 < len(med) and med[j + 1] == med[i]:
        j += 1
    dur = play[j + W] - play[i]
    if dur > 100_000:
        n_steps = j - i + W
        f = n_steps / (dur / 1e6)
        print(f"t={(play[i]-play[0])/1e6:5.2f}s  durée {dur/1e6:4.2f}s  {f:7.1f} Hz  ≈ {note_name(f)}")
    i = j + 1

# --- rendu audio (un clic par pas) --------------------------------------
RATE = 44100
t0 = accepted[0][0]
n = int((accepted[-1][0] - t0) / 1e6 * RATE) + 500
buf = [0.0] * n
click = [math.exp(-k / 30) * math.sin(k * 0.5) for k in range(200)]
for t, _ in accepted:
    s = int((t - t0) / 1e6 * RATE)
    for k, v in enumerate(click):
        if s + k < n:
            buf[s + k] += v
with wave.open("sortie.wav", "w") as w:
    w.setnchannels(1); w.setsampwidth(2); w.setframerate(RATE)
    w.writeframes(b"".join(struct.pack("<h", int(max(-1, min(1, v)) * 20000)) for v in buf))
print("\n→ sortie.wav")
