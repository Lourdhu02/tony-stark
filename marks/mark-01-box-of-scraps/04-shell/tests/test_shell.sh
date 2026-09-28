#!/usr/bin/env bash
# End-to-end tests: feed scripts to stark-shell on stdin, compare stdout.
set -u
cd "$(dirname "$0")/.."
source ../common/test.sh

SHELL_BIN=$(pwd)/build/stark-shell
[ -x "$SHELL_BIN" ] || { echo "missing $SHELL_BIN, run 'make' first"; exit 1; }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
cd "$WORK"

# run 'script': execute a script in a fresh scratch directory.
run() {
    local dir
    dir=$(mktemp -d "$WORK/t.XXXX")
    (cd "$dir" && printf '%s\n' "$1" | HOME="$dir" "$SHELL_BIN")
}
export SHELL_BIN WORK
export -f run

# ── commands and quoting ─────────────────────────────────────────────

check "simple command" "hello" run 'echo hello'
check "arguments" "a b c" run 'echo a   b    c'
check "quotes" "hello   world single | quoted" run "echo \"hello   world\" 'single | quoted'"
check "blank lines are fine" "ok" run $'\n   \necho ok\n'

# ── pipes ────────────────────────────────────────────────────────────

check "two-stage pipe" "HELLO" run 'echo hello | tr a-z A-Z'
check "three-stage pipe" $'a\nb' run 'printf "c\nb\na\n" | sort | head -n 2'
check "pipe with a big payload" "200000" run 'seq 1 200000 | cat | cat | wc -l'

# ── redirection ──────────────────────────────────────────────────────

check "output redirect" "hi" run $'echo hi > f.txt\ncat f.txt'
check "append redirect" $'one\ntwo' run $'echo one > f.txt\necho two >> f.txt\ncat f.txt'
check "input redirect" $'a\nb' run $'printf "b\\na\\n" > in.txt\nsort < in.txt'
check "truncate on >" "new" run $'echo old-and-long > f.txt\necho new > f.txt\ncat f.txt'
check "operators need no spaces" "HI" run $'echo hi>f.txt\ncat<f.txt|tr a-z A-Z'
check "Mark I exit criterion" "3" \
    run $'touch a.c b.c c.c notes.txt\nls | grep \'\\.c$\' | wc -l > out.txt\ncat out.txt'

# ── builtins ─────────────────────────────────────────────────────────

check "cd + pwd" "/" run $'cd /\npwd'
mkdir -p "$WORK/home"
check "cd with no args goes to \$HOME" "$WORK/home" \
    env HOME="$WORK/home" bash -c "cd / && printf 'cd\npwd\n' | '$SHELL_BIN'"
check "bad cd doesn't kill the shell" "alive" run $'cd /definitely/not/here\necho alive'
check "exit with a status" "3" bash -c "printf 'exit 3\necho unreachable\n' | '$SHELL_BIN'; echo \$?"

# ── errors and processes ─────────────────────────────────────────────

check "unknown command keeps going" "after" run $'no-such-command-xyz\necho after'
check "unknown command message" "1" \
    bash -c "printf 'no-such-command-xyz\n' | '$SHELL_BIN' 2>&1 >/dev/null | grep -c 'command not found'"
check "syntax error keeps going" "after" run $'ls |\necho after'
T_TIMEOUT=2 check "background job returns immediately" "quick" run $'sleep 5 > /dev/null &\necho quick'
check "no zombies left behind" "0" \
    run $'sleep 0.1 > /dev/null &\nsleep 0.5\nsh -c \'ps -o stat= --ppid $PPID | grep -c Z\''

# Ctrl-C: a terminal sends SIGINT to the shell AND its foreground child.
# The child must die, the shell must survive.
ctrl_c() {
    local out="$WORK/sigint.out"
    printf 'sleep 5\necho survived\n' | env --default-signal=INT "$SHELL_BIN" > "$out" 2>/dev/null &
    local pid=$!
    sleep 0.5
    pkill -INT -P "$pid" sleep
    kill -INT "$pid" 2>/dev/null
    wait "$pid" 2>/dev/null
    cat "$out"
}
export -f ctrl_c
T_TIMEOUT=4 check "Ctrl-C kills the child, not the shell" "survived" ctrl_c

report "shell"
