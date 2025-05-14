#!/bin/bash

# have to do it this way as we cant modify Makefile

usage() {
    echo "Usage: $0 [--check] [--valgrind] [--afl] [--asan] [--ubsan] [--msan]"
    echo "Options:"
    echo "  --check     Run Check unit tests"
    echo "  --valgrind  Run Valgrind memory checks"
    echo "  --afl       Run AFL fuzzing"
    echo "  --asan      Run with Address Sanitizer"
    echo "  --ubsan     Run with Undefined Behavior Sanitizer"
    echo "  --msan      Run with Memory Sanitizer"
    exit 1
}

if [ $# -eq 0 ]; then
    usage
fi

RUN_CHECK=false
RUN_VALGRIND=false
RUN_AFL=false
RUN_ASAN=false
RUN_UBSAN=false
RUN_MSAN=false

while [ "$1" != "" ]; do
    case $1 in
        --check )    RUN_CHECK=true
                    ;;
        --valgrind ) RUN_VALGRIND=true
                    ;;
        --afl )      RUN_AFL=true
                    ;;
        --asan )     RUN_ASAN=true
                    ;;
        --ubsan )    RUN_UBSAN=true
                    ;;
        --msan )     RUN_MSAN=true
                    ;;
        * )         usage
                    exit 1
    esac
    shift
done

run_check() {
    echo "Running Check unit tests..."
    
    if ! command -v checkmk &> /dev/null; then
        echo "checkmk is not installed. Please install it first."
        exit 1
    fi
    
    for ts_file in src/*.ts; do
        if [ -f "$ts_file" ]; then
            echo "Generating test file from $ts_file"
            checkmk "$ts_file" > "${ts_file%.ts}.c"
        fi
    done
    
    gcc -o bin/test_app \
        $(find src -name "*.c") \
        -I. \
        -lcheck -lm -pthread -lrt -lsubunit \
        -std=c11 -pedantic-errors -Wall -Wextra
    
    if [ $? -ne 0 ]; then
        echo "Failed to compile tests!"
        exit 1
    fi
    
    ./bin/test_app
    TEST_RESULT=$?
    
    if [ $TEST_RESULT -ne 0 ]; then
        echo "Check tests failed!"
        exit 1
    fi
    echo "Check tests completed successfully"
}

run_valgrind() {
    echo "Running Valgrind memory checks..."
    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             --log-file=valgrind-out.txt \
             ./bin/app
    if [ $? -ne 0 ]; then
        echo "Valgrind found issues! Check valgrind-out.txt for details"
        exit 1
    fi
    echo "Valgrind checks completed successfully"
}

run_afl() {
    echo "Running AFL fuzzing..."
    
    if ! command -v afl-fuzz &> /dev/null; then
        echo "AFL is not installed. Please install it first."
        exit 1
    fi
    
    mkdir -p afl_testcases
    mkdir -p afl_findings
    
    if [ ! -f afl_testcases/sample.txt ]; then
        echo "Password123!" > afl_testcases/sample.txt
    fi
    
    afl-fuzz -i afl_testcases -o afl_findings -- ./bin/app @@
}

run_asan() {
    echo "Running with Address Sanitizer..."
    
    gcc -o bin/asan_app \
        $(find src -name "*.c") \
        -I. \
        -fsanitize=address -fno-omit-frame-pointer \
        -std=c11 -pedantic-errors -Wall -Wextra
    
    if [ $? -ne 0 ]; then
        echo "Failed to compile with ASan!"
        exit 1
    fi
    
    ./bin/asan_app
    ASAN_RESULT=$?
    
    if [ $ASAN_RESULT -ne 0 ]; then
        echo "ASan found issues!"
        exit 1
    fi
    echo "ASan checks completed successfully"
}

run_ubsan() {
    echo "Running with Undefined Behavior Sanitizer..."
    
    gcc -o bin/ubsan_app \
        $(find src -name "*.c") \
        -I. \
        -fsanitize=undefined -fno-omit-frame-pointer \
        -std=c11 -pedantic-errors -Wall -Wextra
    
    if [ $? -ne 0 ]; then
        echo "Failed to compile with UBSan!"
        exit 1
    fi
    
    ./bin/ubsan_app
    UBSAN_RESULT=$?
    
    if [ $UBSAN_RESULT -ne 0 ]; then
        echo "UBSan found issues!"
        exit 1
    fi
    echo "UBSan checks completed successfully"
}

run_msan() {
    echo "Running with Memory Sanitizer..."
    
    gcc -o bin/msan_app \
        $(find src -name "*.c") \
        -I. \
        -fsanitize=memory -fno-omit-frame-pointer \
        -std=c11 -pedantic-errors -Wall -Wextra
    
    if [ $? -ne 0 ]; then
        echo "Failed to compile with MSan!"
        exit 1
    fi
    
    ./bin/msan_app
    MSAN_RESULT=$?
    
    if [ $MSAN_RESULT -ne 0 ]; then
        echo "MSan found issues!"
        exit 1
    fi
    echo "MSan checks completed successfully"
}

if [ "$RUN_CHECK" = true ]; then
    run_check
fi

if [ "$RUN_VALGRIND" = true ]; then
    run_valgrind
fi

if [ "$RUN_AFL" = true ]; then
    run_afl
fi

if [ "$RUN_ASAN" = true ]; then
    run_asan
fi

if [ "$RUN_UBSAN" = true ]; then
    run_ubsan
fi

if [ "$RUN_MSAN" = true ]; then
    run_msan
fi

echo "All selected tests completed"