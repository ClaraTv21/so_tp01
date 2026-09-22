#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

set +e
echo "exit 7" | timeout 5 ./sh
code=$?
set -e

if [ "$code" -ne 7 ]; then
    echo "[10] exit did not terminate the shell with the given exit code (expected 7 obtido $code)"
    exit 1
else
    exit 0
fi
