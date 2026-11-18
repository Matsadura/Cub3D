# Dark Maze (cub3D)

A first-person horror maze built on the 42 **cub3D** raycaster (itself
inspired by Wolfenstein 3D). Find every key, reach the exit door, and stay
out of the stalkers' sight. Your flashlight is your only light, and it is
also what gives you away.

**[Play it in your browser](https://matsadura.github.io/Cub3D/)** (keyboard
and mouse, or touch controls on phones and tablets).

| | |
|---|---|
| ![Title screen](docs/screenshots/title.png) | ![A stalker heard you](docs/screenshots/chase.png) |
| ![A key and a battery in front of the exit](docs/screenshots/key.png) | ![The exit door](docs/screenshots/door.png) |

Written in C11 with SDL2 (no game engine): a software raycaster with
flashlight lighting, billboard sprites, a software audio mixer, and the same
code compiled natively and to WebAssembly.

## How to play

- **Find the keys** (`KEYS 0/3` top right), then reach the green **EXIT**
  door. Without every key the door stays locked.
- **The flashlight** (`F`) lights the way but drains its battery; spare
  batteries lie around the maze. With the light on, stalkers see you from
  far away; in the dark they only notice you up close.
- **Sprint** (`Shift`) to outrun a stalker, but it costs breath and makes
  noise: stalkers hear footsteps through the corridors, sprinting far more
  than walking.
- **Stalkers** wake up a few seconds after you arrive, wander, and hunt you
  down the moment they see or hear you. Lose them and they search for a
  while before wandering off again. Your heartbeat and the screen's edges
  tell you how close they are.
- **Three lives** per run; five levels from the Cellar to the Deep. The
  fastest full escape is your best time.

Controls: `W`/`S` move, `A`/`D` strafe, mouse or arrows turn, `Shift`
sprint, `F` flashlight, `Tab` minimap, `P` pause, `R` restart level,
`M` mute, `Esc` quit.

## Build & run

Requires SDL2 and SDL2_image (`sudo apt install libsdl2-dev libsdl2-image-dev`).

```bash
make
./cub3D                      # the campaign (assets/maps/campaign)
./cub3D path/to/map.cub      # your own map(s), played in order
./cub3D --check map.cub      # validate maps without opening a window
```

## Web build

```bash
source ~/emsdk/emsdk_env.sh      # Emscripten 7.0.0
make wasm                        # web/index.{html,js,wasm,data}
python3 -m http.server 8000 -d web
```

In the browser, click the game to look around with the mouse (Esc releases
it and pauses). On touch screens a stick moves, dragging on the game looks
around, and RUN / LIGHT / MAP / OK buttons sit beside it; Fullscreen works
on phones too (with a CSS fallback where the Fullscreen API is missing).
The best time and mute setting are kept in `localStorage`.

**Deployment:** `.github/workflows/pages.yml` builds and publishes the web
version from the `refactor/horror-maze` branch. One-time repository setup:
Settings -> Pages -> Source: *GitHub Actions*, and Settings -> Environments
-> `github-pages` -> Deployment branches: allow `refactor/horror-maze`.
`.github/workflows/ci.yml` builds and tests natively (plus Valgrind) and
builds the WebAssembly version on every push.

## Map format

Standard cub3D `.cub` files still work (they play as free exploration):
`NO`/`SO`/`WE`/`EA` textures (`.png` or `.xpm`), `F`/`C` colours, then the
map with `0` floor, `1` wall, spaces for void and one `N`/`S`/`E`/`W` start.
The horror maze adds four tiles:

| Tile | Meaning |
|------|---------|
| `X`  | Exit door (part of the wall; opens once every key is held) |
| `K`  | Key |
| `B`  | Flashlight battery |
| `M`  | Stalker spawn (at least 4 cells from the player) |

The loader checks that every key and battery and at least one exit can be
reached from the start, and reports errors with line and column.

## Tests

```bash
make test                                   # headless suite
make re DEBUG=1 && VALGRIND=1 ./tests/run_tests.sh
make re DEBUG=1 CC="gcc -fsanitize=address,undefined -fno-omit-frame-pointer"
python3 tests/fuzz_maps.py 3000             # map loader fuzzing
```

All art and sound is generated from code: `python3 tools/gen_assets.py`,
`python3 tools/gen_audio.py` and `python3 tools/gen_maze.py` (campaign).
