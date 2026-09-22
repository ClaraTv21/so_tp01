#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

printf 'export FOO=bar123\nprintenv FOO\n' | timeout 5 ./sh > tests/export.out || true
actual=$(cat tests/export.out)

rm -f tests/export.out

if [ "$actual" != "bar123" ]; then
    echo "[11] export did not make the variable visible to a child process (esperado [bar123] obtido [$actual])"
    exit 1
else
    exit 0
fi
