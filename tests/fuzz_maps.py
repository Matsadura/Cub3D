#!/usr/bin/env python3
"""Mutation fuzzer for the .cub loader (and a quick play test of survivors).

Build with sanitizers first, then run from the repository root:

    make re DEBUG=1 CC="gcc -fsanitize=address,undefined -fno-omit-frame-pointer"
    python3 tests/fuzz_maps.py [iterations] [seed]

Every mutated map must either load (exit 0) or be rejected cleanly (exit 1
with an "Error:" line). Crashes, sanitizer reports or other exit codes are
saved under build/fuzz/ and make the script fail.
"""
import glob
import os
import random
import subprocess
import sys

BIN = "./cub3D"
ALPHABET = b"01NSEW \t\r\n,FCNOSWEA0123456789./\x00\xff"
OUT = "build/fuzz"
ENV = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy",
           ASAN_OPTIONS="detect_leaks=1:abort_on_error=0")


def mutate(rng, data):
    data = bytearray(data)
    for _ in range(rng.randint(1, 8)):
        op = rng.randrange(5)
        pos = rng.randrange(len(data) + 1)
        if op == 0 and data:
            data[min(pos, len(data) - 1)] = rng.choice(ALPHABET)
        elif op == 1:
            data[pos:pos] = bytes([rng.choice(ALPHABET)])
        elif op == 2 and data:
            del data[pos:pos + rng.randint(1, 4)]
        elif op == 3 and data:
            # duplicate a line
            lines = data.split(b"\n")
            i = rng.randrange(len(lines))
            lines.insert(i, lines[i])
            data = bytearray(b"\n".join(lines))
        elif op == 4:
            data[pos:pos] = bytes([rng.randrange(256)])
    return bytes(data)


def run(args):
    p = subprocess.run([BIN] + args, capture_output=True, env=ENV,
                       timeout=30)
    return p.returncode, p.stdout.decode(errors="replace") + \
        p.stderr.decode(errors="replace")


def bad(rc, out):
    return (rc not in (0, 1) or "Sanitizer" in out
            or "runtime error" in out)


def main():
    iterations = int(sys.argv[1]) if len(sys.argv) > 1 else 2000
    rng = random.Random(int(sys.argv[2]) if len(sys.argv) > 2 else 1337)
    seeds = [open(f, "rb").read() for f in
             sorted(glob.glob("tests/maps/valid/*.cub")
                    + glob.glob("assets/maps/*.cub"))]
    os.makedirs(OUT, exist_ok=True)
    path = os.path.join(OUT, "case.cub")
    valid, failures = [], 0
    for i in range(iterations):
        data = mutate(rng, rng.choice(seeds))
        with open(path, "wb") as f:
            f.write(data)
        rc, out = run(["--check", path])
        if bad(rc, out) or (rc == 1 and "Error:" not in out):
            failures += 1
            keep = os.path.join(OUT, "crash_%04d.cub" % i)
            os.replace(path, keep)
            print("FAIL rc=%d %s\n%s" % (rc, keep, out[-2000:]))
        elif rc == 0 and len(valid) < 60:
            keep = os.path.join(OUT, "valid_%04d.cub" % len(valid))
            os.replace(path, keep)
            valid.append(keep)
    for keep in valid:
        keys = "".join(rng.choice("wsadjl<>.rpn") for _ in range(60))
        rc, out = run(["--dt", "0.05", "--keys", keys, "--frames", "60",
                       keep])
        if bad(rc, out) or rc != 0:
            failures += 1
            print("PLAY FAIL rc=%d %s keys=%s\n%s" % (rc, keep, keys,
                                                     out[-2000:]))
    print("fuzzed %d maps (%d valid played), failures: %d"
          % (iterations, len(valid), failures))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
