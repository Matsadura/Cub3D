#!/usr/bin/env python3
"""Generates the five campaign mazes into assets/maps/campaign (deterministic).

    python3 tools/gen_maze.py

Each maze is a recursive-backtracker maze, "braided" (a share of dead ends
knocked through so chases can be escaped around loops) with a few small
rooms. Placement uses breadth-first distances from the player start:
the exit door sits on the outer wall as far away as possible, keys go to
the farthest dead ends (spread apart), stalkers spawn far from the start.
"""
import os
import random
from collections import deque

OUT = os.path.join(os.path.dirname(__file__), "..", "assets", "maps",
                   "campaign")

LEVELS = [
    # name, size, keys, stalkers, batteries, braid, rooms, textures, F, C
    ("cellar", 15, 1, 1, 1, 0.35, 1, ("brick", "brick", "concrete",
                                      "concrete"), (34, 30, 26), (16, 16, 18)),
    ("ward", 19, 2, 1, 2, 0.4, 2, ("tile", "tile", "tile", "concrete"),
     (40, 44, 38), (20, 22, 20)),
    ("archive", 23, 3, 2, 2, 0.4, 3, ("wood", "wood", "brick", "brick"),
     (36, 26, 20), (14, 12, 10)),
    ("tunnels", 27, 3, 2, 3, 0.45, 3, ("concrete", "brick", "concrete",
                                       "brick"), (28, 28, 30), (12, 12, 14)),
    ("the_deep", 31, 4, 3, 3, 0.5, 4, ("brick", "tile", "wood", "concrete"),
     (24, 20, 20), (10, 8, 8)),
]

DIRS = ((0, -1), (1, 0), (0, 1), (-1, 0))


def carve(n, rng):
    g = [["1"] * n for _ in range(n)]
    stack = [(1, 1)]
    g[1][1] = "0"
    while stack:
        x, y = stack[-1]
        nxt = [(dx, dy) for dx, dy in DIRS
               if 0 < x + 2 * dx < n - 1 and 0 < y + 2 * dy < n - 1
               and g[y + 2 * dy][x + 2 * dx] == "1"]
        if not nxt:
            stack.pop()
            continue
        dx, dy = rng.choice(nxt)
        g[y + dy][x + dx] = "0"
        g[y + 2 * dy][x + 2 * dx] = "0"
        stack.append((x + 2 * dx, y + 2 * dy))
    return g


def open_neighbours(g, x, y):
    return [(x + dx, y + dy) for dx, dy in DIRS if g[y + dy][x + dx] == "0"]


def dead_ends(g):
    n = len(g)
    return [(x, y) for y in range(1, n - 1) for x in range(1, n - 1)
            if g[y][x] == "0" and len(open_neighbours(g, x, y)) == 1]


def braid(g, share, rng):
    n = len(g)
    for x, y in dead_ends(g):
        if rng.random() > share:
            continue
        walls = [(dx, dy) for dx, dy in DIRS
                 if g[y + dy][x + dx] == "1"
                 and 0 < x + 2 * dx < n - 1 and 0 < y + 2 * dy < n - 1]
        if walls:
            dx, dy = rng.choice(walls)
            g[y + dy][x + dx] = "0"


def rooms(g, count, rng):
    n = len(g)
    for _ in range(count):
        cx, cy = rng.randrange(3, n - 4, 2), rng.randrange(3, n - 4, 2)
        for y in range(cy - 1, cy + 2):
            for x in range(cx - 1, cx + 2):
                g[y][x] = "0"


def bfs(g, start):
    dist = {start: 0}
    q = deque([start])
    while q:
        c = q.popleft()
        for nb in open_neighbours(g, *c):
            if nb not in dist:
                dist[nb] = dist[c] + 1
                q.append(nb)
    return dist


def spread_pick(cands, count, dist, avoid):
    """Greedy farthest-point picks: far from the start and from each other."""
    picks = []
    for _ in range(count):
        best, score = None, -1
        for c in cands:
            if c in picks or c in avoid:
                continue
            near = min([abs(c[0] - p[0]) + abs(c[1] - p[1])
                        for p in picks + list(avoid)] or [99])
            s = dist[c] + 2 * min(near, 12)
            if s > score:
                best, score = c, s
        if best is None:
            break
        picks.append(best)
    return picks


def place_exit(g, dist):
    n = len(g)
    best, far = None, -1
    for (x, y), d in dist.items():
        for dx, dy in DIRS:
            wx, wy = x + dx, y + dy
            if (wx in (0, n - 1) or wy in (0, n - 1)) and d > far \
                    and 0 < wx + wy and not (wx in (0, n - 1)
                                             and wy in (0, n - 1)):
                best, far = (wx, wy), d
    g[best[1]][best[0]] = "X"
    return best


def build(level, seed):
    name, n, keys, stalkers, batteries, share, nrooms, tex, floor, ceil = level
    rng = random.Random(seed)
    g = carve(n, rng)
    braid(g, share, rng)
    rooms(g, nrooms, rng)
    start = (1, 1)
    dist = bfs(g, start)
    door = place_exit(g, dist)
    ends = [c for c in dead_ends(g) if dist.get(c, 0) > n // 3]
    key_cells = spread_pick(ends or list(dist), keys, dist, {start, door})
    far = [c for c, d in dist.items()
           if d >= max(dist.values()) * 0.45
           and (c[0] - 1) ** 2 + (c[1] - 1) ** 2 >= 36]
    stalker_cells = spread_pick(far, stalkers, dist, {start, *key_cells})
    mid = [c for c, d in dist.items()
           if max(dist.values()) * 0.2 <= d <= max(dist.values()) * 0.7]
    used = {start, *key_cells, *stalker_cells}
    battery_cells = [c for c in rng.sample(mid, min(len(mid), 40))
                     if c not in used][:batteries]
    for x, y in key_cells:
        g[y][x] = "K"
    for x, y in stalker_cells:
        g[y][x] = "M"
    for x, y in battery_cells:
        g[y][x] = "B"
    g[1][1] = "S" if g[2][1] == "0" else "E"
    head = ["NO assets/textures/%s.png" % tex[0],
            "SO assets/textures/%s.png" % tex[1],
            "WE assets/textures/%s.png" % tex[2],
            "EA assets/textures/%s.png" % tex[3],
            "",
            "F %d,%d,%d" % floor,
            "C %d,%d,%d" % ceil,
            ""]
    return "\n".join(head + ["".join(r) for r in g]) + "\n"


def main():
    os.makedirs(OUT, exist_ok=True)
    for i, level in enumerate(LEVELS):
        path = os.path.join(OUT, "%02d_%s.cub" % (i + 1, level[0]))
        with open(path, "w") as f:
            f.write(build(level, 1000 + i))
        print(path)


if __name__ == "__main__":
    main()
