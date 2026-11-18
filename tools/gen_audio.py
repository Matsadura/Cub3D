#!/usr/bin/env python3
"""Synthesises the horror-maze sounds into assets/sounds (original audio).

Pure stdlib, deterministic:   python3 tools/gen_audio.py

Everything is built from sines, saws, filtered noise and envelopes: a low
looping drone for the maze, a sparse title theme, heartbeat, footsteps,
the stalker's growl and scream, and short stingers. 22050 Hz mono WAV;
loops 8-bit, effects 16-bit (the mixer converts on load).
"""
import math
import os
import random
import struct
import wave

RATE = 22050
OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "sounds")
TAU = 2 * math.pi


def buffer(seconds):
    return [0.0] * int(seconds * RATE)


def env_ad(t, attack, decay):
    if t < attack:
        return t / attack
    return math.exp(-(t - attack) / decay)


def add_tone(buf, start, dur, f, vol, attack=0.01, decay=None, wave_="sin",
             slide_to=None, vibrato=0.0, vib_rate=5.0):
    s0, n = int(start * RATE), int(dur * RATE)
    phase = 0.0
    for i in range(n):
        t = i / RATE
        cur = f if slide_to is None else f + (slide_to - f) * (i / n)
        if vibrato:
            cur *= 1.0 + vibrato * math.sin(TAU * vib_rate * t)
        phase = (phase + cur / RATE) % 1.0
        if wave_ == "sin":
            v = math.sin(TAU * phase)
        elif wave_ == "saw":
            v = 2.0 * phase - 1.0
        else:
            v = 1.0 if phase < 0.5 else -1.0
        e = env_ad(t, attack, decay) if decay else min(1.0, t / attack)
        tail = dur - t
        if tail < 0.02:
            e *= tail / 0.02
        idx = s0 + i
        if 0 <= idx < len(buf):
            buf[idx] += vol * e * v


def add_noise(buf, start, dur, vol, cutoff=0.1, attack=0.005, decay=None,
              seed=1, am=None):
    """One-pole low-passed white noise; `am` = (rate, depth) tremolo."""
    rng = random.Random(seed)
    s0, n = int(start * RATE), int(dur * RATE)
    y = 0.0
    for i in range(n):
        t = i / RATE
        y += cutoff * (rng.uniform(-1, 1) - y)
        e = env_ad(t, attack, decay) if decay else min(1.0, t / attack)
        if am:
            e *= 1.0 - am[1] * (0.5 + 0.5 * math.sin(TAU * am[0] * t))
        tail = dur - t
        if tail < 0.02:
            e *= tail / 0.02
        idx = s0 + i
        if 0 <= idx < len(buf):
            buf[idx] += vol * e * y * 3.0


def write_wav(name, buf, peak=0.85, bits=16):
    top = max(1e-9, max(abs(x) for x in buf))
    scale = min(1.0, peak / top)
    clamp = [max(-1.0, min(1.0, x * scale)) for x in buf]
    if bits == 8:
        frames = bytes(int(round(x * 127)) + 128 for x in clamp)
    else:
        frames = b"".join(struct.pack("<h", int(x * 32767)) for x in clamp)
    with wave.open(os.path.join(OUT, name + ".wav"), "wb") as w:
        w.setnchannels(1)
        w.setsampwidth(bits // 8)
        w.setframerate(RATE)
        w.writeframes(frames)
    return len(buf) / RATE


def loop_fade(buf, seconds=0.25):
    """Crossfades the tail into the head so the loop has no click."""
    n = int(seconds * RATE)
    for i in range(n):
        k = i / n
        buf[i] = buf[i] * k + buf[len(buf) - n + i] * (1 - k)
    return buf[:len(buf) - n]


# ------------------------------------------------------------------ loops

def amb_drone():
    secs = 12.0
    buf = buffer(secs + 0.25)
    for f, v in ((41.0, 0.30), (41.7, 0.25), (61.5, 0.12), (82.4, 0.06)):
        add_tone(buf, 0, secs + 0.25, f, v, attack=0.5)
    add_noise(buf, 0, secs + 0.25, 0.10, cutoff=0.015, attack=0.5, seed=3,
              am=(0.11, 0.7))
    # distant metal groans
    for t, f in ((2.5, 180.0), (7.3, 140.0), (10.1, 210.0)):
        add_tone(buf, t, 1.6, f, 0.05, attack=0.4, decay=0.7, wave_="saw",
                 slide_to=f * 0.82, vibrato=0.01)
    return loop_fade(buf)


def music_title():
    secs = 16.0
    buf = buffer(secs + 0.25)
    for f, v in ((55.0, 0.22), (55.6, 0.18), (82.4, 0.10)):
        add_tone(buf, 0, secs + 0.25, f, v, attack=1.0)
    bells = [(0.5, 440.0), (2.5, 415.3), (4.5, 349.2), (6.5, 329.6),
             (8.5, 440.0), (10.5, 466.2), (12.5, 392.0), (14.5, 329.6)]
    for t, f in bells:
        for mult, v in ((1.0, 0.12), (2.76, 0.04), (5.4, 0.02)):
            add_tone(buf, t, 2.0, f * mult, v, attack=0.005, decay=0.6)
    add_noise(buf, 0, secs + 0.25, 0.05, cutoff=0.02, attack=1.0, seed=8)
    return loop_fade(buf)


# ------------------------------------------------------------------ effects

def sfx_step():
    buf = buffer(0.16)
    add_noise(buf, 0, 0.15, 0.6, cutoff=0.08, attack=0.002, decay=0.035,
              seed=11)
    add_tone(buf, 0, 0.1, 70, 0.3, attack=0.002, decay=0.03)
    return buf


def sfx_heartbeat():
    buf = buffer(0.5)
    for t, v in ((0.0, 0.9), (0.18, 0.6)):
        add_tone(buf, t, 0.16, 52, v, attack=0.004, decay=0.05,
                 slide_to=38)
        add_noise(buf, t, 0.08, 0.15, cutoff=0.05, attack=0.002,
                  decay=0.02, seed=int(t * 100) + 5)
    return buf


def sfx_key():
    buf = buffer(0.7)
    for i, t in enumerate((0.0, 0.07, 0.15)):
        for f, v in ((1320.0, 0.10), (2210.0, 0.06), (3580.0, 0.04)):
            add_tone(buf, t, 0.5, f * (1 + 0.03 * i), v, attack=0.002,
                     decay=0.12)
    return buf


def sfx_battery():
    buf = buffer(0.45)
    add_noise(buf, 0, 0.03, 0.5, cutoff=0.5, attack=0.001, seed=21)
    add_tone(buf, 0.05, 0.35, 120, 0.2, attack=0.05, decay=0.2,
             slide_to=240, wave_="saw")
    return buf


def sfx_locked():
    buf = buffer(0.55)
    for i, t in enumerate((0.0, 0.12, 0.26)):
        add_noise(buf, t, 0.1, 0.6, cutoff=0.25, attack=0.001, decay=0.03,
                  seed=31 + i)
        add_tone(buf, t, 0.1, 160 + 20 * i, 0.25, attack=0.001, decay=0.04,
                 wave_="sqr")
    return buf


def sfx_door():
    buf = buffer(1.6)
    add_tone(buf, 0, 1.1, 90, 0.18, attack=0.1, wave_="saw", slide_to=150,
             vibrato=0.04, vib_rate=11.0)
    add_noise(buf, 0, 1.1, 0.1, cutoff=0.1, attack=0.1, seed=41,
              am=(9.0, 0.8))
    add_noise(buf, 1.15, 0.4, 0.9, cutoff=0.2, attack=0.001, decay=0.12,
              seed=42)
    add_tone(buf, 1.15, 0.4, 55, 0.6, attack=0.001, decay=0.15)
    return buf


def sfx_spotted():
    buf = buffer(1.4)
    for f in (622.0, 659.0, 698.0, 932.0):
        add_tone(buf, 0, 1.3, f, 0.09, attack=0.01, decay=0.6, wave_="saw",
                 vibrato=0.015, vib_rate=7.0)
    add_tone(buf, 0, 0.6, 55, 0.4, attack=0.002, decay=0.25)
    return buf


def sfx_growl():
    buf = buffer(1.5)
    add_tone(buf, 0, 1.4, 58, 0.35, attack=0.2, decay=0.6, wave_="saw",
             slide_to=46, vibrato=0.06, vib_rate=17.0)
    add_noise(buf, 0, 1.4, 0.35, cutoff=0.04, attack=0.2, decay=0.6,
              seed=51, am=(23.0, 0.9))
    return buf


def sfx_scream():
    buf = buffer(1.3)
    for f in (880.0, 1244.0, 1661.0):
        add_tone(buf, 0, 1.2, f, 0.14, attack=0.005, decay=0.5, wave_="saw",
                 slide_to=f * 0.45, vibrato=0.03, vib_rate=13.0)
    add_noise(buf, 0, 1.2, 0.5, cutoff=0.6, attack=0.002, decay=0.4,
              seed=61)
    add_tone(buf, 0, 0.8, 40, 0.6, attack=0.002, decay=0.4)
    return buf


def sfx_light():
    buf = buffer(0.08)
    add_noise(buf, 0, 0.02, 0.7, cutoff=0.6, attack=0.0005, seed=71)
    add_tone(buf, 0, 0.05, 2600, 0.15, attack=0.0005, decay=0.01)
    return buf


def sfx_light_dead():
    buf = buffer(0.9)
    for i in range(6):
        add_noise(buf, i * 0.12, 0.05, 0.4 - i * 0.05, cutoff=0.5,
                  attack=0.001, seed=81 + i)
    add_tone(buf, 0, 0.8, 300, 0.1, attack=0.01, decay=0.3, slide_to=60,
             wave_="saw")
    return buf


def sfx_breath():
    buf = buffer(1.2)
    for t in (0.0, 0.55):
        add_noise(buf, t, 0.45, 0.5, cutoff=0.12, attack=0.12, decay=0.2,
                  seed=int(t * 10) + 91)
    return buf


def sfx_pause():
    buf = buffer(0.25)
    add_tone(buf, 0, 0.2, 220, 0.3, attack=0.005, decay=0.08)
    return buf


def jingle_escape():
    buf = buffer(2.5)
    for f in (220.0, 277.2, 329.6, 440.0):
        add_tone(buf, 0.05, 2.3, f, 0.12, attack=0.4, decay=1.0)
    return buf


def jingle_game_over():
    buf = buffer(3.0)
    for i, f in enumerate((110.0, 103.8, 98.0, 82.4)):
        add_tone(buf, i * 0.5, 1.4, f, 0.3, attack=0.02, decay=0.6,
                 wave_="saw")
    add_noise(buf, 0, 2.8, 0.1, cutoff=0.02, attack=0.5, seed=99)
    return buf


def jingle_victory():
    buf = buffer(3.2)
    for i, f in enumerate((220.0, 329.6, 440.0, 554.4, 659.3)):
        add_tone(buf, i * 0.25, 2.4, f, 0.12, attack=0.05, decay=1.0)
    return buf


def main():
    os.makedirs(OUT, exist_ok=True)
    loops = {"amb_drone": amb_drone(), "music_title": music_title()}
    effects = {
        "sfx_step": sfx_step(), "sfx_heartbeat": sfx_heartbeat(),
        "sfx_key": sfx_key(), "sfx_battery": sfx_battery(),
        "sfx_locked": sfx_locked(), "sfx_door": sfx_door(),
        "sfx_spotted": sfx_spotted(), "sfx_growl": sfx_growl(),
        "sfx_scream": sfx_scream(), "sfx_light": sfx_light(),
        "sfx_light_dead": sfx_light_dead(), "sfx_breath": sfx_breath(),
        "sfx_pause": sfx_pause(), "jingle_escape": jingle_escape(),
        "jingle_game_over": jingle_game_over(),
        "jingle_victory": jingle_victory(),
    }
    for name, buf in sorted(loops.items()):
        print("%-22s %6.2fs" % (name + ".wav", write_wav(name, buf, bits=8)))
    for name, buf in sorted(effects.items()):
        print("%-22s %6.2fs" % (name + ".wav", write_wav(name, buf)))


if __name__ == "__main__":
    main()
