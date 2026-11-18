#!/usr/bin/env python3
"""Generates the horror-maze textures and sprites (original art, from code).

Pure stdlib (zlib + struct), deterministic:   python3 tools/gen_assets.py

Art direction: 64x64 pixel art, desaturated grime (sickly greens, rust,
bruised browns) at mid brightness, because the renderer does all of the
lighting (fog, flashlight cone). Pixels with alpha 254 are "emissive": the
renderer draws them at full brightness even in total darkness (the EXIT
sign, the stalker's eyes, glints on keys and batteries).
"""
import math
import os
import struct
import zlib

ROOT = os.path.join(os.path.dirname(__file__), "..", "assets")
TEX = os.path.join(ROOT, "textures")
SPR = os.path.join(ROOT, "sprites")
GLOW = 254
CLEAR = (0, 0, 0, 0)


# ------------------------------------------------------------------ helpers

def png_write(path, rows):
    h, w = len(rows), len(rows[0])
    raw = b"".join(b"\x00" + b"".join(bytes(px) for px in row) for row in rows)

    def chunk(tag, data):
        return (struct.pack(">I", len(data)) + tag + data
                + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF))

    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n")
        f.write(chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0)))
        f.write(chunk(b"IDAT", zlib.compress(raw, 9)))
        f.write(chunk(b"IEND", b""))


def hash2(x, y, seed):
    h = (x * 374761393 + y * 668265263 + seed * 2246822519) & 0xFFFFFFFF
    h = ((h ^ (h >> 13)) * 1274126177) & 0xFFFFFFFF
    return ((h ^ (h >> 16)) & 0xFFFF) / 65535.0


def vnoise(x, y, scale, seed, wrap=64):
    """Tileable value noise in [0, 1]."""
    period = max(1, wrap // scale)
    fx, fy = x / scale, y / scale
    x0, y0 = int(math.floor(fx)), int(math.floor(fy))
    tx, ty = fx - x0, fy - y0
    tx, ty = tx * tx * (3 - 2 * tx), ty * ty * (3 - 2 * ty)

    def c(i, j):
        return hash2(i % period, j % period, seed)
    a = c(x0, y0) + (c(x0 + 1, y0) - c(x0, y0)) * tx
    b = c(x0, y0 + 1) + (c(x0 + 1, y0 + 1) - c(x0, y0 + 1)) * tx
    return a + (b - a) * ty


def fbm(x, y, seed, octaves=(16, 8, 4, 2)):
    total, amp, norm = 0.0, 1.0, 0.0
    for s in octaves:
        total += vnoise(x, y, s, seed + s) * amp
        norm += amp
        amp *= 0.55
    return total / norm


def mix(c1, c2, t):
    t = max(0.0, min(1.0, t))
    return tuple(int(round(a + (b - a) * t)) for a, b in zip(c1, c2))


def shade(c, k):
    return tuple(max(0, min(255, int(round(v * k)))) for v in c[:3])


def opaque(c):
    return (c[0], c[1], c[2], 255)


def canvas(w, h, fill=CLEAR):
    return [[fill] * w for _ in range(h)]


def save(dirname, name, rows):
    os.makedirs(dirname, exist_ok=True)
    png_write(os.path.join(dirname, name + ".png"), rows)
    print("%-28s %dx%d" % (os.path.join(os.path.basename(dirname),
                                        name + ".png"),
                           len(rows[0]), len(rows)))


def stains(x, y, seed, base):
    """Dark vertical drips and grime blotches over a colour."""
    grime = fbm(x, y, seed + 7)
    drip = vnoise(x, 0, 4, seed + 3)
    drip_len = drip * 48
    k = 0.75 + 0.5 * grime
    if drip > 0.72 and y < drip_len:
        k *= 0.55 + 0.3 * (y / max(1.0, drip_len))
    return shade(base, k)


# ------------------------------------------------------------------ textures

def tex_brick(seed=11):
    rows = canvas(64, 64)
    mortar = (62, 60, 52)
    for y in range(64):
        for x in range(64):
            course = y // 8
            off = 8 if course % 2 else 0
            bx = (x + off) % 16
            brick_id = ((x + off) // 16, course)
            if y % 8 == 7 or bx == 15:
                c = mortar
            else:
                tint = hash2(brick_id[0], brick_id[1], seed)
                c = mix((92, 70, 58), (70, 76, 62), tint)
                if bx == 0 or y % 8 == 0:
                    c = shade(c, 1.12)
            rows[y][x] = opaque(stains(x, y, seed, c))
    return rows


def tex_concrete(seed=23):
    rows = canvas(64, 64)
    for y in range(64):
        for x in range(64):
            n = fbm(x, y, seed)
            c = mix((70, 72, 68), (104, 104, 96), n)
            if y in (31, 32) or (x == 31 and y > 32):
                c = shade(c, 0.7)  # panel seams
            crack = abs(vnoise(x, y, 8, seed + 99) - 0.5)
            if crack < 0.018:
                c = shade(c, 0.45)
            rows[y][x] = opaque(stains(x, y, seed, c))
    return rows


def tex_tile(seed=37):
    rows = canvas(64, 64)
    for y in range(64):
        for x in range(64):
            if x % 16 == 15 or y % 16 == 15:
                c = (58, 62, 54)
            else:
                t = hash2(x // 16, y // 16, seed)
                c = mix((150, 162, 140), (122, 140, 120), t)
                if hash2(x // 16, y // 16, seed + 5) > 0.85:
                    c = shade(c, 0.6)  # broken tile
            c = stains(x, y, seed, c)
            if y > 40:  # filth rising from the floor
                c = shade(c, 1.0 - (y - 40) / 64.0 * fbm(x, y, seed + 1))
            rows[y][x] = opaque(c)
    return rows


def tex_wood(seed=51):
    rows = canvas(64, 64)
    for y in range(64):
        for x in range(64):
            plank = x // 13
            grain = math.sin((y + hash2(plank, 0, seed) * 40) * 0.35
                             + vnoise(x, y, 8, seed) * 6.0)
            c = mix((66, 46, 34), (98, 70, 48), 0.5 + 0.5 * grain)
            c = shade(c, 0.85 + 0.3 * hash2(plank, 1, seed))
            if x % 13 == 12:
                c = (30, 22, 18)
            if y in (10, 54) and x % 13 in (3, 9):
                c = (40, 38, 36)  # nails
            rows[y][x] = opaque(stains(x, y, seed, c))
    return rows


def tex_door(seed=67):
    rows = canvas(64, 64)
    for y in range(64):
        for x in range(64):
            n = fbm(x, y, seed)
            c = mix((58, 64, 66), (84, 90, 92), n)  # steel
            if x < 4 or x > 59 or y < 3:
                c = (34, 30, 28)  # frame
            elif x in (8, 55) or y in (36, 37):
                c = shade(c, 0.7)
            if 42 <= x <= 47 and 40 <= y <= 43:
                c = (150, 140, 110)  # push bar handle
            rust = vnoise(x, y, 4, seed + 4)
            if rust > 0.78:
                c = mix(c, (120, 60, 30), (rust - 0.78) * 4)
            rows[y][x] = opaque(c)
    # glowing EXIT sign (emissive)
    sign = ["XXX.X.X.XXX.XXX",
            "X...X.X..X...X.",
            "XX...X...X...X.",
            "X...X.X..X...X.",
            "XXX.X.X.XXX..X."]
    for y in range(9, 22):
        for x in range(12, 52):
            rows[y][x] = (10, 60, 20, GLOW)
    for j, line in enumerate(sign):
        for i, ch in enumerate(line):
            if ch == "X":
                for dy in range(2):
                    for dx in range(2):
                        rows[11 + j * 2 + dy][17 + i * 2 + dx] = \
                            (120, 255, 140, GLOW)
    return rows


# ------------------------------------------------------------------ sprites

def blit_art(rows, art, palette, ox, oy, scale=1):
    for j, line in enumerate(art):
        for i, ch in enumerate(line):
            if ch in palette:
                for dy in range(scale):
                    for dx in range(scale):
                        y, x = oy + j * scale + dy, ox + i * scale + dx
                        if 0 <= y < len(rows) and 0 <= x < len(rows[0]):
                            rows[y][x] = palette[ch]


def spr_key():
    art = ["....OOOO........",
           "...O#..#O.......",
           "..O#....#O......",
           "..O#....#O......",
           "...O#..#O.......",
           "....O##O........",
           ".....O#O........",
           ".....O#O........",
           ".....O#OOO......",
           ".....O####O.....",
           ".....O#OOO......",
           ".....O#O........",
           ".....O##OO......",
           ".....O####O.....",
           "......OOOO......",
           "................"]
    pal = {"O": (70, 44, 20, 255), "#": (190, 150, 60, 255)}
    rows = canvas(64, 64)
    blit_art(rows, art, pal, 16, 28, 2)
    for x, y in ((24, 32), (26, 34), (28, 52)):  # glints
        rows[y][x] = (255, 240, 170, GLOW)
    return rows


def spr_battery():
    rows = canvas(64, 64)
    for y in range(30, 62):
        for x in range(24, 40):
            edge = x in (24, 39) or y in (30, 61)
            band = 34 <= y <= 40
            c = (30, 30, 34) if edge else ((170, 120, 30) if band
                                          else (60, 62, 70))
            if x in (27, 28) and not edge:
                c = shade(c, 1.4)
            rows[y][x] = opaque(c)
    for y in range(27, 30):
        for x in range(29, 35):
            rows[y][x] = (150, 150, 150, 255)
    for y in range(46, 56):
        for x in range(30, 34):
            rows[y][x] = (90, 255, 110, GLOW)
    return rows


STALKER_BODY = [
    "......................",
    ".........####.........",
    "........######........",
    ".......##E##E##.......",
    ".......########.......",
    "........#MMMM#........",
    ".........####.........",
    "......##########......",
    ".....############.....",
    "....##############....",
    "...###.########.###...",
    "...##..########..##...",
    "..##...########...##..",
    "..##...########...##..",
    ".##....########....##.",
    ".##....########....##.",
    ".#.....########.....#.",
    ".#......######......#.",
    "##......######......##",
    "#.......##..##.......#",
    "........##..##........",
    "........##..##........",
    "........##..##........",
    ".......###..###.......",
    "......###....###......",
]


def spr_stalker(frame):
    rows = canvas(64, 64)
    art = [list(r) for r in STALKER_BODY]
    if frame == 1:  # arms swing, legs apart
        for r in (16, 17, 18, 19):
            art[r] = list(art[r][1:] + ["."])
        art[23] = list("......###....###.......")[:22]
        art[24] = list(".....###......###......")[:22]
    pal = {"#": (22, 20, 24, 255), "M": (60, 10, 12, 255),
           "E": (255, 236, 220, GLOW)}
    for j, line in enumerate(art):
        for i, ch in enumerate(line):
            if ch in pal:
                c = pal[ch]
                if ch == "#":  # faint rim light so the shape reads
                    c = opaque(shade(c, 1.0 + 0.8 * fbm(i * 3, j * 3, 5)))
                for dy in range(2):
                    for dx in range(3):
                        x, y = 9 + i * 2 + dx // 2 * 0 + dx, 13 + j * 2 + dy
                        if x < 64 and y < 64 and dx < 2:
                            rows[y][x] = c
    return rows


def spr_face():
    """The jump-scare close-up: gaunt grey skin, black sockets with pin-
    point pupils, a jaw stretched far too wide."""
    rows = canvas(64, 64)
    for y in range(64):
        for x in range(64):
            dx = (x - 32) / (24.0 - max(0, y - 30) * 0.28)
            dy = (y - 30) / 32.0
            if dx * dx + dy * dy <= 1.0 and y < 63:
                n = fbm(x, y, 91)
                c = mix((70, 74, 66), (138, 140, 124), n)
                hollow = abs(abs(x - 32) - 15) < 4 and 34 < y < 46
                c = shade(c, (0.55 if hollow else 1.0)
                          * (1.0 - 0.5 * (dx * dx + dy * dy)))
                vein = vnoise(x, y, 4, 17)
                if 0.49 < vein < 0.51:
                    c = mix(c, (60, 20, 30), 0.6)
                rows[y][x] = opaque(c)
    for cx in (20, 44):
        for y in range(14, 34):
            for x in range(cx - 9, cx + 10):
                e = ((x - cx) / 9.0) ** 2 + ((y - 24) / 10.0) ** 2
                if e <= 1.0:
                    rows[y][x] = opaque(shade((40, 34, 34), 0.25 + 0.4 * e))
        for y in range(23, 26):
            for x in range(cx - 1, cx + 2):
                rows[y][x] = (255, 236, 200, GLOW)
    for y in range(38, 64):
        half = int(10 * math.sin(min(1.0, (y - 38) / 22.0) * math.pi * 0.9))
        for x in range(32 - half, 32 + half + 1):
            if 0 <= x < 64:
                rows[y][x] = (14, 4, 6, 255)
        if y in (39, 40) or y in (56, 57):
            for x in range(32 - half, 32 + half + 1, 3):
                if 0 <= x < 64:
                    rows[y][x] = (176, 168, 140, 255)  # teeth
    return rows


def spr_hand():
    """Player's fist gripping the flashlight (bottom-right overlay)."""
    w, h = 96, 72
    rows = canvas(w, h)
    skin_d, skin_l = (112, 76, 58), (160, 116, 90)
    ax, ay = 0.8, 0.6            # barrel axis (pointing up-left from 20,10)

    def barrel(x, y):
        u = (x - 20) * ax + (y - 10) * ay
        v = -(x - 20) * ay + (y - 10) * ax
        return u, v
    for y in range(h):
        for x in range(w):
            u, v = barrel(x, y)
            if 0 <= u <= 74 and -7 <= v <= 7:
                k = 1.0 - abs(v + 2) / 10.0
                c = (int(55 * k + 18), int(58 * k + 18), int(66 * k + 22))
                if u < 7:
                    c = (190, 190, 160) if u > 2 else (120, 120, 104)
                elif int(u) % 9 == 0 and u < 30:
                    c = shade(c, 0.6)  # grip ridges
                rows[y][x] = opaque(c)
    # sleeve, entering from the bottom-right corner
    for y in range(h):
        for x in range(w):
            if (x - 96) * 0.55 + (y - 72) * -0.9 < 22 and x + y > 120:
                n = fbm(x, y, 21)
                rows[y][x] = opaque(mix((30, 32, 40), (48, 50, 62), n))
    # four fingers wrapped over the barrel, thumb on top
    for f in range(4):
        cu = 40 + f * 7.0
        for y in range(h):
            for x in range(w):
                u, v = barrel(x, y)
                du, dv = (u - cu) / 3.6, (v - 2.0) / 9.0
                if du * du + dv * dv <= 1.0:
                    k = 0.75 + 0.35 * (1.0 - abs(du))
                    rows[y][x] = opaque(shade(mix(skin_d, skin_l,
                                                  fbm(x, y, 9)), k))
    for y in range(h):
        for x in range(w):
            u, v = barrel(x, y)
            du, dv = (u - 34) / 9.0, (v + 6.5) / 3.2
            if du * du + dv * dv <= 1.0:
                rows[y][x] = opaque(shade(skin_l, 0.9 + 0.2 * (1 - abs(du))))
    for y in range(3, 18):
        for x in range(9, 23):
            if ((x - 15.5) / 6.0) ** 2 + ((y - 10) / 6.0) ** 2 <= 1.0:
                rows[y][x] = (255, 250, 210, GLOW)  # lens
    return rows


def main():
    save(TEX, "brick", tex_brick())
    save(TEX, "concrete", tex_concrete())
    save(TEX, "tile", tex_tile())
    save(TEX, "wood", tex_wood())
    save(TEX, "door", tex_door())
    save(SPR, "key", spr_key())
    save(SPR, "battery", spr_battery())
    save(SPR, "stalker_0", spr_stalker(0))
    save(SPR, "stalker_1", spr_stalker(1))
    save(SPR, "face", spr_face())
    save(SPR, "hand", spr_hand())


if __name__ == "__main__":
    main()
