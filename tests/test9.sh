#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

target=$(mktemp -d)
expected=$(readlink -f "$target")

printf 'cd %s\npwd\n' "$target" | timeout 5 ./sh > tests/cd.out || true
actual=$(cat tests/cd.out)

rm -rf "$target"

if [ "$actual" != "$expected" ]; then
    echo "[9] cd did not change the shell's own directory (expected [$expected] obtido [$actual])"
    rm -f tests/cd.out
    exit 1
else
    rm -f tests/cd.out
    exit 0
fi
