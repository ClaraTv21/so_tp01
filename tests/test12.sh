#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

printf 'export SAUDACAO=OperacionaisXYZ\necho ola-$SAUDACAO-fim\n' | timeout 5 ./sh > tests/var.out || true
actual=$(cat tests/var.out)

rm -f tests/var.out

if [ "$actual" != "ola-OperacionaisXYZ-fim" ]; then
    echo "[12] \$VAR nao foi expandida corretamente (esperado [ola-OperacionaisXYZ-fim] obtido [$actual])"
    exit 1
else
    exit 0
fi
