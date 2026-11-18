This project is inspired by the world-famous Wolfenstein 3D game, which is
considered the first FPS ever created.

## Build & run

Requires SDL2 and SDL2_image (`sudo apt install libsdl2-dev libsdl2-image-dev`).

```bash
make
./cub3D                      # built-in maps (assets/maps)
./cub3D path/to/map.cub      # your own map(s)
./cub3D --check map.cub      # validate maps without opening a window
```

Controls: `W`/`S` move, `A`/`D` strafe, arrows or mouse to turn, `Tab`
minimap, `P` pause, `R` restart, `Esc` quit.

## Tests

```bash
make test                                   # headless suite
make re DEBUG=1 && VALGRIND=1 ./tests/run_tests.sh
make re DEBUG=1 CC="gcc -fsanitize=address,undefined -fno-omit-frame-pointer"
python3 tests/fuzz_maps.py 3000             # map loader fuzzing
```
