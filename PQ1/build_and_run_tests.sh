#!/bin/bash

# Select compiler: use gcc-13 if available, otherwise gcc
CC=gcc; command -v gcc-13 >/dev/null && CC=gcc-13

CFLAGS="-std=c17 -Wall -Wextra -Werror -pedantic -g -I."
SRCS="tl_semaphore.c"

mkdir -p output

FAILED=0

compile()
{
    local test_src=$1
    local out=$2
    local extra_flags=$3
    echo "Compiling $test_src..."
    $CC $CFLAGS $extra_flags $test_src $SRCS -o $out
    if [ $? -ne 0 ]; then
        echo "FAIL: compilation of $test_src failed"
        FAILED=1
        return 1
    fi
    return 0
}

run_test()
{
    local bin=$1
    echo "Running $bin..."
    $bin
    if [ $? -ne 0 ]; then
        echo "FAIL: $bin"
        FAILED=1
    else
        echo "PASS: $bin"
    fi
}

compile "tests/test_unit.c"       "output/test_unit"       ""          && run_test "./output/test_unit"
compile "tests/test_concurrent.c" "output/test_concurrent" "-pthread"  && run_test "./output/test_concurrent"
compile "tests/test_stress.c"     "output/test_stress"     "-pthread"  && run_test "./output/test_stress"

if [ $FAILED -eq 0 ]; then
    echo ""
    echo "All tests passed."
    exit 0
else
    echo ""
    echo "One or more tests failed."
    exit 1
fi
