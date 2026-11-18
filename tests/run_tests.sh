#!/usr/bin/env bash
# Headless regression tests for the .cub loader, the horror-maze rules,
# rendering and audio.
# Usage: tests/run_tests.sh            (VALGRIND=1 to run every case under valgrind)
set -u
cd "$(dirname "$0")/.."

BIN=./cub3D
SUPP="$(pwd)/tests/valgrind.supp"
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
			--suppressions="$SUPP" --gen-suppressions=all "$@"
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
for m in tests/maps/valid/*.cub tests/maps/valid/rel/*.cub assets/maps/*/*.cub \
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

echo "== movement (scripted input, dummy video driver, fixed steps)"
# Keys: w/s forward/back, a/d strafe, j/l turn, z sprint forward, f light,
# < > mouse look, e enter, r restart, p pause, n minimap, m mute, q quit,
# . wait one frame. Every run starts on the title screen, hence "e".
play() { # name expected_status_regex args...
	local name=$1 expect=$2; shift 2
	out=$(run "$BIN" --dt 0.1 "$@" 2>&1); rc=$?
	if [ $rc -eq 0 ] && grep -qE -- "$expect" <<<"$out"; then ok "$name"
	else ko "$name" "rc=$rc $(tail -n 3 <<<"$out")"; fi
}
rep() { printf "%${2}s" '' | tr ' ' "$1"; }  # rep CHAR COUNT
V=tests/maps/valid
MIN=$V/min.cub          # 2x2 room, spawn (1.5,1.5) facing north
TOP=$V/test_pos_top.cub # 1-wide N-S corridor, spawn at the top
COR=$V/corridor.cub     # long E-W corridor, spawn facing east

play "starts on the title screen"    "level=1 state=TITLE lives=3 keys=0/1" --frames 3
play "enter starts the campaign"     "level=1 state=PLAYING lives=3 keys=0/1" --keys "e" --frames 3
play "spawn faces north"             "x=1.50 y=1.50 angle=270.0" --keys "e" --frames 2 $MIN
play "wall stops forward walk flush" "x=1.50 y=1.20 " --keys "e$(rep w 5)" --frames 7 $MIN
play "wall stops backward walk"      "x=1.50 y=2.(79|80) " --keys "e$(rep s 8)" --frames 10 $MIN
play "strafe right goes east"        "x=2.(79|80) y=1.50 " --keys "e$(rep d 8)" --frames 10 $MIN
play "turn right 0.5s = 63 deg"      "angle=333.0" --keys "e$(rep l 5)" --frames 7 $MIN
play "mouse look 100px = 14.3 deg"   "angle=284.3" --keys "e>" --frames 3 $MIN
play "walls slide, no clipping"      "x=1.(20|21) y=1.(20|21) " \
	--keys "e$(rep j 6)$(rep w 24)" --frames 32 $MIN
play "walk 1 s = 3 tiles"            "x=4.50 .*stamina=1.00" --keys "e$(rep w 10)" --frames 12 $COR
play "sprint 1 s = 5.1 tiles"        "x=6.60 .*stamina=0.7[0-9]" --keys "e$(rep z 10)" --frames 12 $COR
play "sprint until out of breath"    "stamina=0.0[0-9]" --keys "e$(rep z 36)" --frames 38 $COR
play "breath comes back"             "stamina=0.[3-9]" --keys "e$(rep z 36)$(rep . 30)" --frames 68 $COR
play "pause freezes everything"      "state=PAUSED .*time=0.10 .*stalker=dormant@11.50" \
	--keys "e.p$(rep s 50)" --frames 55 $V/chase.cub
play "enter resumes from pause"      "state=PLAYING .*x=1.50 y=3.00 " --keys "epe$(rep s 5)" --frames 9 $TOP
play "tab toggles the minimap"       "minimap=0" --keys "en" --frames 3 $MIN
play "ESC/quit exits cleanly"        "state=PLAYING" --keys "eq" $MIN

echo "== keys, door, flashlight"
play "key opens the exit door"       "state=ESCAPED .*keys=1/1" --keys "e$(rep w 18)" --frames 20 $V/key_door.cub
play "door stays locked without key" "state=PLAYING .*keys=0/1" --keys "e$(rep w 18)" --frames 20 $V/locked_door.cub
play "fetch the key, then escape"    "state=ESCAPED .*keys=1/1" \
	--keys "e$(rep w 18)$(rep s 20)$(rep w 20)" --frames 60 $V/locked_door.cub
play "door without keys is open"     "state=ESCAPED .*keys=0/0" --keys "e$(rep w 18)" --frames 20 $V/exit_open.cub
play "last exit -> victory + best"   "state=VICTORY" --keys "e$(rep w 18)e" --frames 21 $V/key_door.cub
play "escape -> next level"          "level=2 state=PLAYING lives=3 keys=0/1 x=1.50" \
	--keys "e$(rep w 18)e" --frames 21 $V/key_door.cub $V/key_door.cub
play "victory -> title"              "state=TITLE" --keys "e$(rep w 18)e.e" --frames 23 $V/key_door.cub
play "restart drops held keys"       "keys=0/1 x=1.50 .*time=0.00" --keys "e$(rep w 8)r" --frames 10 $V/key_door.cub
play "flashlight drains 1/150 per s" "battery=0.90 light=1" --keys "e$(rep . 150)" --frames 152 $MIN
play "light off saves the battery"   "battery=1.00 light=0" --keys "ef$(rep . 150)" --frames 153 $MIN
play "battery runs out"              "battery=0.00 light=0" --keys "e$(rep . 1505)" --norender --frames 1507 $MIN
play "dead light cannot turn on"     "battery=0.00 light=0" --keys "e$(rep . 1505)f" --norender --frames 1508 $MIN
play "battery pickup refills"        "battery=1.00 light=1" --keys "e$(rep . 300)$(rep w 8)" --norender --frames 310 $V/battery.cub
play "no pickup, battery stays low"  "battery=0.80" --keys "e$(rep . 300)" --norender --frames 302 $V/battery.cub

echo "== stalkers"
D30=$(rep . 30)
play "dormant during grace"          "stalker=dormant@11.50,1.50" --keys "e" --frames 25 $V/chase.cub
play "hunts the lit player"          "state=PLAYING .*stalker=hunt" --keys "e" --frames 45 $V/chase.cub
play "does not see in the dark"      "light=0 .*stalker=wander" --keys "ef" --frames 60 $V/chase.cub
play "walls block its sight"         "light=1 .*stalker=wander" --keys "e" --frames 60 $V/stealth.cub
play "walking is not heard"          "stalker=wander" --keys "ef${D30}$(rep w 5)" --frames 40 $V/hearing.cub
play "sprinting is heard"            "stalker=hunt" --keys "ef${D30}$(rep z 5)" --frames 40 $V/hearing.cub
play "caught = jump scare"           "state=CAUGHT lives=3" --keys "e" --frames 80 $V/chase.cub
play "life lost after the scare"     "state=PLAYING lives=2 .*stalker=dormant@11.50" --keys "e" --frames 110 $V/chase.cub
play "three catches = game over"     "state=GAME_OVER lives=0" --keys "e" --norender --frames 300 $V/chase.cub
play "game over -> new run"          "state=PLAYING lives=3" --keys "e$(rep . 298)e" --norender --frames 301 $V/chase.cub
# Frame-rate independence: same chase outcome at 60 Hz and 240 Hz.
play "free at 7.7s @60Hz"   "state=PLAYING .*stalker=hunt@2.[01]" --dt 0.0166666667 --norender --frames 463 --keys e $V/chase.cub
play "free at 7.7s @240Hz"  "state=PLAYING .*stalker=hunt@2.[01]" --dt 0.0041666667 --norender --frames 1849 --keys e $V/chase.cub
play "caught by 7.8s @60Hz"  "state=CAUGHT" --dt 0.0166666667 --norender --frames 469 --keys e $V/chase.cub
play "caught by 7.8s @240Hz" "state=CAUGHT" --dt 0.0041666667 --norender --frames 1873 --keys e $V/chase.cub

echo "== audio"
play "all sounds load"               "sounds=18 muted=0" --keys "e" --frames 3
out=$(run "$BIN" --dt 0.1 --keys "e" --frames 3 2>&1)
grep -q "Warning" <<<"$out" && ko "no audio warnings" "$out" || ok "no audio warnings"
play "M toggles mute"                "muted=1" --keys "em" --frames 3
play "M again unmutes"               "muted=0" --keys "em.m" --frames 5
play "every sound event plays"       "state=TITLE" \
	--keys "e$(rep w 8)fpp$(rep z 30)$(rep w 30)e.e" --frames 80 $V/key_door.cub
mkdir -p "$TMP/game" && cp -r assets "$TMP/game/" && rm "$TMP/game/assets/sounds/sfx_key.wav"
out=$(cd "$TMP/game" && run "$OLDPWD/$BIN" --dt 0.1 --keys "e" --frames 3 2>&1); rc=$?
if [ $rc -eq 0 ] && grep -q "Warning: cannot load assets/sounds/sfx_key.wav" <<<"$out" \
	&& grep -q "sounds=17" <<<"$out"; then ok "missing sound is not fatal"
else ko "missing sound is not fatal" "rc=$rc $out"; fi

echo "== rendering"
out=$(run "$BIN" --dt 0.1 --frames 2 --screenshot "$TMP/shot.bmp" 2>&1); rc=$?
if [ $rc -eq 0 ] && [ -s "$TMP/shot.bmp" ]; then ok "screenshot of a frame"
else ko "screenshot of a frame" "rc=$rc $out"; fi
for m in assets/maps/*/*.cub tests/maps/valid/*.cub; do
	out=$(run "$BIN" --dt 0.05 --keys "e$(rep w 4)$(rep l 5)$(rep z 5)$(rep j 3)fw.f$(rep w 4)n" \
		--frames 30 "$m" 2>&1); rc=$?
	[ $rc -eq 0 ] && ok "renders + walks: $m" || ko "renders + walks: $m" "rc=$rc $(tail -n 2 <<<"$out")"
done

echo
echo "passed: $pass  failed: $fail"
[ $fail -eq 0 ]
