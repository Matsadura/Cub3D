#!/usr/bin/env bash
# Headless regression tests for the .cub loader and the core game loop.
# Usage: tests/run_tests.sh            (VALGRIND=1 to run every case under valgrind)
set -u
cd "$(dirname "$0")/.."

BIN=./cub3D
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-dummy}"
export SDL_AUDIODRIVER=dummy
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
pass=0
fail=0

run() {
	if [ "${VALGRIND:-0}" = 1 ]; then
		valgrind -q --leak-check=full \
			--errors-for-leak-kinds=definite,indirect,possible \
			--track-fds=yes --error-exitcode=99 --num-callers=40 \
			--suppressions=tests/valgrind.supp --gen-suppressions=all "$@"
	else
		"$@"
	fi
}

ok()   { pass=$((pass + 1)); printf '  \033[32mPASS\033[0m %s\n' "$1"; }
ko()   {
	fail=$((fail + 1))
	printf '  \033[31mFAIL\033[0m %s\n' "$1"
	[ -n "${2:-}" ] && printf '       %s\n' "$2"
	if [ "${VALGRIND:-0}" = 1 ] && grep -q '^==' <<<"${out:-}"; then
		printf '%s\n' "$out" | sed 's/^/       | /'
	fi
	return 0
}

# Generated fixtures (too big or impossible to commit).
mkdir -p "$TMP/dir.cub"
python3 - "$TMP/oversize.cub" <<'EOF'
import sys
with open(sys.argv[1], "w") as f:
    f.write("NO assets/textures/no.png\n" + "\n" * (1 << 20))
EOF

echo "== valid maps (expect OK)"
for m in tests/maps/valid/*.cub tests/maps/valid/rel/*.cub assets/maps/*.cub \
	tests/maps/runtime/*.cub; do
	out=$(run "$BIN" --check "$m" 2>&1); rc=$?
	[ $rc -eq 0 ] && ok "$m" || ko "$m" "$out"
done

echo "== invalid maps (expect clean error, exit 1)"
for m in tests/maps/invalid/*.cub tests/maps/bad_names/* tests/maps/bad_names/.cub \
	"$TMP/dir.cub" "$TMP/oversize.cub" tests/maps/does_not_exist.cub; do
	out=$(run "$BIN" --check "$m" 2>&1); rc=$?
	if [ $rc -eq 1 ] && grep -q "Error:" <<<"$out"; then ok "${m#$TMP/} -> ${out#*Error: }"
	else ko "$m" "rc=$rc $out"; fi
done

echo "== command line"
expect_fail() { # name args...
	local name=$1; shift
	out=$(run "$BIN" "$@" 2>&1); rc=$?
	if [ $rc -eq 1 ] && grep -qE "$EXPECT" <<<"$out"; then ok "$name"
	else ko "$name" "rc=$rc $(tail -n 2 <<<"$out")"; fi
}
EXPECT="Usage"; expect_fail "--check without maps prints usage" --check
EXPECT="Usage"; expect_fail "unknown option prints usage" --bogus
EXPECT="Error: .*not enclosed"; expect_fail "invalid map refuses to start" \
	--frames 2 tests/maps/invalid/wall_none.cub
EXPECT="Error: cannot load texture"; expect_fail "corrupt texture fails cleanly" \
	--frames 2 tests/maps/runtime/corrupt_texture.cub

echo "== gameplay (scripted input, dummy video driver, fixed steps)"
# Keys: w/s forward/back, a/d strafe, j/l turn, < > mouse look, e enter,
# r restart, p pause, n minimap, q quit, . wait one frame.
play() { # name expected_status_regex args...
	local name=$1 expect=$2; shift 2
	out=$(run "$BIN" --dt 0.1 "$@" 2>&1); rc=$?
	if [ $rc -eq 0 ] && grep -qE -- "$expect" <<<"$out"; then ok "$name"
	else ko "$name" "rc=$rc $(tail -n 3 <<<"$out")"; fi
}
MIN=tests/maps/valid/min.cub      # 2x2 room, spawn (1.5,1.5) facing north
TOP=tests/maps/valid/test_pos_top.cub  # 1-wide N-S corridor, spawn at the top

play "built-in maps load"            "level=1 state=PLAYING x=26.50 y=11.50" --frames 3
play "spawn faces north"             "x=1.50 y=1.50 angle=270.0" --frames 1 $MIN
play "wall stops forward walk flush" "x=1.50 y=1.20 " --keys "wwwww" --frames 6 $MIN
play "wall stops backward walk"      "x=1.50 y=2.(79|80) " --keys "ssssssss" --frames 9 $MIN
play "strafe right goes east"        "x=2.(79|80) y=1.50 " --keys "dddddddd" --frames 9 $MIN
play "strafe left blocked by wall"   "x=1.20 y=1.50 " --keys "aaaaa" --frames 6 $MIN
play "turn right 0.5s = 63 deg"      "angle=333.0" --keys "lllll" --frames 6 $MIN
play "turn left 0.5s = 63 deg"       "angle=207.0" --keys "jjjjj" --frames 6 $MIN
play "mouse look 100px = 14.3 deg"   "angle=284.3" --keys ">" --frames 2 $MIN
play "mouse look left"               "angle=255.7" --keys "<" --frames 2 $MIN
play "corridor walk 1.5 tiles"       "x=1.50 y=3.00 " --keys "sssss" --frames 6 $TOP
play "walls slide, no clipping"      "x=1.(20|21) y=1.(20|21) " \
	--keys "jjjjjjwwwwwwwwwwwwwwwwwwwwwwww" --frames 32 $MIN
play "pause freezes the player"      "state=PAUSED x=1.50 y=1.50 .*time=0.00" \
	--keys "psssss" --frames 7 $TOP
play "enter resumes from pause"      "state=PLAYING x=1.50 y=3.00 " \
	--keys "pesssss" --frames 8 $TOP
play "P toggles pause off"           "state=PLAYING" --keys "p.p" --frames 4 $TOP
play "restart returns to spawn"      "x=1.50 y=1.50 angle=270.0 .*time=0.00" \
	--keys "ssslllr" --frames 7 $TOP
play "tab toggles the minimap"       "minimap=0" --keys "n" --frames 2 $MIN
play "minimap toggles back"          "minimap=1" --keys "n.n" --frames 4 $MIN
play "ESC/quit exits cleanly"        "level=1" --keys "q" $MIN

# Frame-rate independence: 0.5 s of walking and turning gives the same
# result at 60 Hz and 240 Hz.
play "walk 0.5s @60Hz"   "y=3.00 " --dt 0.0166666667 --keys "$(printf 's%.0s' {1..30})" --frames 31 $TOP
play "walk 0.5s @240Hz"  "y=3.00 " --dt 0.0041666667 --keys "$(printf 's%.0s' {1..120})" --frames 121 $TOP
play "turn 0.5s @60Hz"   "angle=333.0" --dt 0.0166666667 --keys "$(printf 'l%.0s' {1..30})" --frames 31 $MIN
play "turn 0.5s @240Hz"  "angle=333.0" --dt 0.0041666667 --keys "$(printf 'l%.0s' {1..120})" --frames 121 $MIN

echo "== rendering"
out=$(run "$BIN" --dt 0.1 --frames 2 --screenshot "$TMP/shot.bmp" 2>&1); rc=$?
if [ $rc -eq 0 ] && [ -s "$TMP/shot.bmp" ]; then ok "screenshot of a frame"
else ko "screenshot of a frame" "rc=$rc $out"; fi
for m in tests/maps/valid/*.cub assets/maps/*.cub; do
	out=$(run "$BIN" --dt 0.05 --keys "wwwwlllllwwwwwjjjwwww" --frames 25 "$m" 2>&1); rc=$?
	[ $rc -eq 0 ] && ok "renders + walks: $m" || ko "renders + walks: $m" "rc=$rc $(tail -n 2 <<<"$out")"
done

echo
echo "passed: $pass  failed: $fail"
[ $fail -eq 0 ]
