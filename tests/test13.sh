#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

printf 'echo antes-$NAOEXISTE123-depois\necho ainda-vivo\n' | timeout 5 ./sh > tests/var2.out || true
actual=$(cat tests/var2.out)
expected=$(printf 'antes--depois\nainda-vivo')

rm -f tests/var2.out

if [ "$actual" != "$expected" ]; then
    echo "[13] \$VAR inexistente nao virou string vazia, ou o shell nao sobreviveu ao comando seguinte (esperado [$expected] obtido [$actual])"
    exit 1
else
    exit 0
fi
