#!/usr/bin/env bash
# Run real programs on your allocator via LD_PRELOAD and compare their
# output with the same programs on glibc's malloc.
set -u
cd "$(dirname "$0")/.."
source ../common/test.sh

LIB=$(pwd)/build/libstarkmalloc.so
[ -f "$LIB" ] || { echo "missing $LIB, run 'make' first"; exit 1; }

STARK=(env LD_PRELOAD="$LIB")

check "ls -la /usr/bin" "$(ls -la /usr/bin)" "${STARK[@]}" ls -la /usr/bin

INPUT=$(seq 1 200000 | awk '{ print ($1 * 7919) % 200003 }')
check "sort 200k lines" "$(sort -n <<<"$INPUT")" "${STARK[@]}" sort -n <<<"$INPUT"

check "awk word count" "$(seq 1 50000 | awk '{ w[$1 % 1000]++ } END { print length(w) }')" \
    "${STARK[@]}" awk '{ w[$1 % 1000]++ } END { print length(w) }' < <(seq 1 50000)

check "bash string building" "48894" \
    "${STARK[@]}" bash -c 'x=""; for i in $(seq 1 10000); do x+="$i "; done; echo ${#x}'

if command -v python3 >/dev/null; then
    check "python3 dict churn" "4999950000 100000" \
        "${STARK[@]}" python3 -c 'd = {i: str(i) * 3 for i in range(100000)}; print(sum(d), len(d))'
fi

report "preload"
