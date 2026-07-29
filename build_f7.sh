#!/usr/bin/env bash

set -u

JOBS=${JOBS:-$(nproc)}

SUCCESS=()
FAILED=()

TARGETS=$(find src/config/configs -name config.h | while read cfg; do
    if grep -q "STM32F7" "$cfg"; then
        basename "$(dirname "$cfg")"
    fi
done | sort)

TOTAL=$(echo "$TARGETS" | wc -l)
COUNT=1

for TARGET in $TARGETS; do
    echo
    echo "=========================================================="
    echo "[$COUNT/$TOTAL] Building $TARGET"
    echo "=========================================================="

    if make -j${JOBS} "$TARGET"; then
        SUCCESS+=("$TARGET")
    else
        FAILED+=("$TARGET")
    fi

    COUNT=$((COUNT+1))
done

echo
echo "=================== F7 Summary ==================="
echo "Success : ${#SUCCESS[@]}"
printf '  ✓ %s\n' "${SUCCESS[@]}"

echo
echo "Failed  : ${#FAILED[@]}"
printf '  ✗ %s\n' "${FAILED[@]}"

[ ${#FAILED[@]} -eq 0 ]