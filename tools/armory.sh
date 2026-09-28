#!/usr/bin/env bash
# armory.sh: run every project's tests and print the suit-status dashboard.
#
#   tools/armory.sh                  all Marks
#   tools/armory.sh marks/mark-01-*  specific Marks
#   tools/armory.sh -v               also show individual test output
#   SAN=1 tools/armory.sh            build with ASan + UBSan
#
# Exit status is non-zero only if a project listed in its Mark's COMPLETED
# file fails. Projects still in progress report but never break the build.
set -u
cd "$(dirname "$0")/.."

VERBOSE=0
if [ "${1:-}" = "-v" ]; then VERBOSE=1; shift; fi
MARKS=("$@")
[ ${#MARKS[@]} -eq 0 ] && MARKS=(marks/mark-*/)

if [ -t 1 ]; then
    G=$'\033[32m' B=$'\033[1;32m' D=$'\033[2;32m' R=$'\033[31m' Y=$'\033[33m' X=$'\033[0m'
else
    G='' B='' D='' R='' Y='' X=''
fi

bar() { # bar PASS TOTAL WIDTH
    local w=$3 f=0 s='' i
    [ "$2" -gt 0 ] && f=$(($1 * w / $2))
    for ((i = 0; i < w; i++)); do if [ $i -lt $f ]; then s+='█'; else s+='░'; fi; done
    printf '%s' "$s"
}

SUMMARY=${GITHUB_STEP_SUMMARY:-/dev/null}
broken=0 grand_pass=0 grand_total=0 prev_locked=0

printf '\n%s  ┌──────────────────────────────────────────────────────────┐%s\n' "$D" "$X"
printf '%s  │%s %sSTARK INDUSTRIES%s %s// ARMORY DIAGNOSTICS%s                   %s│%s\n' "$D" "$X" "$B" "$X" "$D" "$X" "$D" "$X"
printf '%s  └──────────────────────────────────────────────────────────┘%s\n' "$D" "$X"
{
    echo "## STARK INDUSTRIES // ARMORY DIAGNOSTICS"
    echo
} >> "$SUMMARY"

for mark in "${MARKS[@]}"; do
    mark=${mark%/}
    [ -d "$mark" ] || continue
    name=$(basename "$mark")                      # mark-01-box-of-scraps
    num=$(cut -d- -f2 <<<"$name")                 # 01
    title=$(cut -d- -f3- <<<"$name" | tr '-' ' ' | tr '[:lower:]' '[:upper:]')
    [ "$title" = "DUM E" ] && title="DUM-E"
    completed=$(grep -v '^#' "$mark/COMPLETED" 2>/dev/null || true)

    shopt -s nullglob
    projects=("$mark"/[0-9][0-9]-*/)
    shopt -u nullglob
    if [ ${#projects[@]} -eq 0 ]; then # a dossier with no code yet
        [ "$prev_locked" -eq 0 ] && echo
        printf '  %sMARK %s · %-28s locked%s\n' "$D" "$num" "$title" "$X"
        prev_locked=1
        echo "*MARK $num · $title: locked*" >> "$SUMMARY"
        echo >> "$SUMMARY"
        continue
    fi

    prev_locked=0
    printf '\n  %sMARK %s%s %s· %s%s\n\n' "$B" "$num" "$X" "$D" "$title" "$X"
    {
        echo "### MARK $num · $title"
        echo
        echo '| system | integrity | tests | status |'
        echo '|---|---|---|---|'
    } >> "$SUMMARY"

    for proj in "${projects[@]}"; do
        proj=${proj%/}
        pname=$(basename "$proj")
        out=$(make -s -C "$proj" test 2>&1)
        pass=$(awk '/^RESULT /{ s += $2 } END { print s + 0 }' <<<"$out")
        total=$(awk '/^RESULT /{ s += $3 } END { print s + 0 }' <<<"$out")
        grand_pass=$((grand_pass + pass))
        grand_total=$((grand_total + total))

        if [ "$total" -eq 0 ]; then status="${R}BUILD FAILED${X}" plain="BUILD FAILED"
        elif [ "$pass" -eq "$total" ]; then status="${B}ONLINE${X}" plain="ONLINE"
        elif [ "$pass" -eq 0 ]; then status="${D}OFFLINE${X}" plain="OFFLINE"
        else status="${Y}CALIBRATING${X}" plain="CALIBRATING"
        fi

        is_done=0
        grep -qx "$pname" <<<"$completed" && is_done=1
        if [ $is_done -eq 1 ] && { [ "$total" -eq 0 ] || [ "$pass" -ne "$total" ]; }; then
            status="${R}REGRESSION${X}" plain="REGRESSION"
            broken=1
        fi

        printf '  %s%-12s%s %s%s%s %3d/%-3d %s\n' "$G" "$pname" "$X" "$G" "$(bar "$pass" "$total" 24)" "$X" "$pass" "$total" "$status"
        echo "| \`$pname\` | \`$(bar "$pass" "$total" 20)\` | $pass/$total | $plain |" >> "$SUMMARY"

        if [ $VERBOSE -eq 1 ] || [ "$plain" = "REGRESSION" ] || [ "$plain" = "BUILD FAILED" ]; then
            grep -v '^RESULT ' <<<"$out" | sed 's/^/      /'
        fi
    done
    echo >> "$SUMMARY"
done

pct=0
[ "$grand_total" -gt 0 ] && pct=$((100 * grand_pass / grand_total))
printf '\n  %s%s%s  %s%d%%%s  %s%d/%d systems nominal%s\n\n' "$G" "$(bar "$grand_pass" "$grand_total" 40)" "$X" "$B" "$pct" "$X" "$D" "$grand_pass" "$grand_total" "$X"
echo "**$grand_pass/$grand_total tests passing ($pct%)**" >> "$SUMMARY"

if [ $broken -ne 0 ]; then
    printf '  %s✗ a COMPLETED system regressed%s\n\n' "$R" "$X"
    exit 1
fi
