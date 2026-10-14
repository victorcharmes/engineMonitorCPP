#!/usr/bin/env bash
# Tests de bout en bout : execute le programme sur chaque scenario et compare
# la sortie standard au fichier attendu (tests/expected/<nom>.out).
#
# Usage : tests/run_tests.sh <chemin_du_binaire> [nom_de_scenario...]
#   ex  : tests/run_tests.sh bin/monitor
#         tests/run_tests.sh bin/monitor 02_alarms

set -u

if [ $# -lt 1 ]; then
    echo "usage: $0 <binaire> [scenario...]" >&2
    exit 2
fi

BIN="$1"
shift
ROOT="$(cd "$(dirname "$0")" && pwd)"
SCEN_DIR="$ROOT/scenarios"
EXP_DIR="$ROOT/expected"

if [ ! -x "$BIN" ]; then
    echo "binaire introuvable ou non executable : $BIN" >&2
    exit 2
fi

if [ $# -gt 0 ]; then
    names=("$@")
else
    names=()
    for f in "$SCEN_DIR"/*.txt; do
        names+=("$(basename "$f" .txt)")
    done
fi

pass=0
fail=0
tmp="$(mktemp)"
trap 'rm -f "$tmp"' EXIT

for name in "${names[@]}"; do
    scenario="$SCEN_DIR/$name.txt"
    expected="$EXP_DIR/$name.out"

    if [ ! -f "$scenario" ] || [ ! -f "$expected" ]; then
        echo "[ ?? ] $name : scenario ou sortie attendue manquant"
        fail=$((fail + 1))
        continue
    fi

    "$BIN" "$scenario" > "$tmp" 2>/dev/null
    status=$?

    if [ $status -gt 1 ]; then
        echo "[FAIL] $name : code de retour $status (plantage ?)"
        fail=$((fail + 1))
    elif diff -u "$expected" "$tmp" > /dev/null; then
        echo "[ OK ] $name"
        pass=$((pass + 1))
    else
        echo "[FAIL] $name : sortie differente (- attendu, + obtenu)"
        diff -u --label attendu --label obtenu "$expected" "$tmp" | sed 's/^/       /'
        fail=$((fail + 1))
    fi
done

echo "Scenarios : $pass OK, $fail en echec"
[ $fail -eq 0 ]
