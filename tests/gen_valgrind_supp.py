#!/usr/bin/env python3
"""Writes tests/valgrind.supp: suppressions for errors raised *inside* SDL2's
own start-up (driver probing, CPU-feature SIMD paths, D-Bus/IME, audio device
thread). Anything the game allocates or touches afterwards is still reported.

Verify after generating: inject a real leak in game code (e.g. drop a free()
in the map loader) and confirm `VALGRIND=1 ./tests/run_tests.sh` still fails.

    python3 gen_valgrind_supp.py > tests/valgrind.supp
"""

KINDS = ["Leak", "Cond", "Value8", "Value4", "Addr1", "Addr2", "Addr4",
         "Addr8", "Addr16", "Param", "Overlap", "Free"]
ENTRY_POINTS = ["SDL_Init*", "SDL_InitSubSystem*", "SDL_VideoInit*",
                "IMG_Init*", "SDL_CreateWindow*", "SDL_CreateRenderer*",
                "SDL_OpenAudioDevice*"]

print("# Valgrind suppressions for third-party SDL2 start-up internals only.")
for fn in ENTRY_POINTS:
    for kind in KINDS:
        print("{")
        print("   sdl_startup_%s_%s" % (fn.rstrip("*").lower(), kind.lower()))
        print("   Memcheck:" + kind)
        if kind == "Leak":
            print("   match-leak-kinds: definite,indirect,possible")
        if kind == "Param":
            print("   *")
        print("   ...")
        print("   fun:" + fn)
        print("   ...")
        print("}")
