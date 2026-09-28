#!/usr/bin/env bash
# test.sh: the shell twin of test.h, for integration tests.
#
#   source ../common/test.sh
#   check "name" "expected output" command args...
#   check_cmd "name" command args...        # passes if the command exits 0
#   report "suite"
#
# Each check runs with a timeout so a hung program can't stall the suite.
# CMD may be a helper function if you `export -f` it (and its variables).

T_PASS=0
T_TOTAL=0
T_TIMEOUT=${T_TIMEOUT:-10}

if [ -t 1 ]; then
    T_G=$'\033[32m' T_R=$'\033[31m' T_D=$'\033[2m' T_0=$'\033[0m'
else
    T_G='' T_R='' T_D='' T_0=''
fi

# Run "$@" (a program or an exported function) under a timeout.
t_exec() { timeout "$T_TIMEOUT" bash -c '"$@"' t_exec "$@"; }

t_ok()   { T_PASS=$((T_PASS + 1)); T_TOTAL=$((T_TOTAL + 1)); printf '  %s[ OK ]%s %s\n' "$T_G" "$T_0" "$1"; }
t_fail() { T_TOTAL=$((T_TOTAL + 1)); printf '  %s[FAIL]%s %s %s(%s)%s\n' "$T_R" "$T_0" "$1" "$T_D" "$2" "$T_0"; }

# check NAME EXPECTED CMD...: stdout of CMD must equal EXPECTED exactly.
check() {
    local name=$1 want=$2 got
    shift 2
    got=$(t_exec "$@" 2>/dev/null)
    local rc=$?
    if [ $rc -eq 124 ]; then t_fail "$name" "timeout"
    elif [ "$got" == "$want" ]; then t_ok "$name"
    else
        t_fail "$name" "output mismatch"
        printf '         want: %q\n         got:  %q\n' "$want" "$got"
    fi
}

# check_cmd NAME CMD...: CMD must exit 0.
check_cmd() {
    local name=$1
    shift
    t_exec "$@" >/dev/null 2>&1
    local rc=$?
    if [ $rc -eq 0 ]; then t_ok "$name"
    elif [ $rc -eq 124 ]; then t_fail "$name" "timeout"
    elif [ $rc -gt 128 ]; then t_fail "$name" "killed by signal $((rc - 128))"
    else t_fail "$name" "exit $rc"
    fi
}

report() {
    local width=24 filled=0 bar='' i
    [ "$T_TOTAL" -gt 0 ] && filled=$((T_PASS * width / T_TOTAL))
    for ((i = 0; i < width; i++)); do
        if [ $i -lt $filled ]; then bar+='█'; else bar+='░'; fi
    done
    printf '\n  %s%-8s%s %s %d/%d\n' "$T_G" "$1" "$T_0" "$bar" "$T_PASS" "$T_TOTAL"
    echo "RESULT $T_PASS $T_TOTAL"
    [ "$T_PASS" -eq "$T_TOTAL" ]
}
