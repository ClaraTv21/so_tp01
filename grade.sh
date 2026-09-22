#!/bin/bash
set -u

srcfn="sh.c"
total=13
ecnt=0

RESET='\033[0m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'

print_result() {
    if [ "$1" -eq 0 ]; then
        echo -e "${GREEN}PASS${RESET}"
    else
        echo -e "${RED}FAIL${RESET}"
    fi
}

run_test() {
    local n="$1" desc="$2"
    echo -e "${BLUE}Running Test ${n}: ${desc}...${RESET}"
    if ! "tests/test${n}.sh" "$srcfn"; then
        ecnt=$(( ecnt + 1 ))
        print_result 1
    else
        print_result 0
    fi
}

run_test 1 "Simple ls command"
run_test 2 "Sequence of simple ls commands"
run_test 3 "Output redirection command"
run_test 4 "Input redirection command"
run_test 5 "Input/output redirection command"
run_test 6 "Pipe command"
run_test 7 "Sequence of pipe commands"
run_test 8 "Sequence of pipe commands with output redirection"
run_test 9  "cd muda o diretorio do proprio shell (persiste entre linhas)"
run_test 10 "exit encerra o shell com o codigo informado"
run_test 11 "export realmente exporta para o ambiente dos filhos (setenv)"
run_test 12 "expansao de \$VAR definida via export"
run_test 13 "expansao de \$VAR inexistente vira string vazia (nao quebra o shell)"

# Summary
echo -e "${YELLOW}Test Summary:${RESET}"
echo -e "${GREEN}Your code passed $(( total - ecnt )) of ${total} tests.${RESET}"

if [ "$ecnt" -gt 0 ]; then
    exit 1
fi
exit 0
