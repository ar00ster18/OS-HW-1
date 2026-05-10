#!/bin/bash

# Select compiler: use gcc-13 if available, otherwise gcc
CC=gcc; command -v gcc-13 >/dev/null && CC=gcc-13

CFLAGS="-std=c17 -Wall -Wextra -Werror -pedantic -g -I."
SRCS="tl_semaphore.c cond_var.c rw_lock.c"

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

# Extra unit test
compile "tests/test_rwlock_unit.c"             "output/test_rwlock_unit"             ""         && run_test "./output/test_rwlock_unit"

# Required tests
compile "tests/test_rw_readers_parallel.c"        "output/test_rw_readers_parallel"        "-pthread" && run_test "./output/test_rw_readers_parallel"
compile "tests/test_rw_writer_exclusive.c"        "output/test_rw_writer_exclusive"        "-pthread" && run_test "./output/test_rw_writer_exclusive"
compile "tests/test_rw_writer_preference_stress.c" "output/test_rw_writer_preference_stress" "-pthread" && run_test "./output/test_rw_writer_preference_stress"

if [ $FAILED -eq 0 ]; then
    echo ""
    echo "All tests passed."
    exit 0
else
    echo ""
    echo "One or more tests failed."
    exit 1
fi
