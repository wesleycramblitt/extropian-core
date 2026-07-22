#!/usr/bin/env bash
# Run a specific test binary (or list available tests).
# Usage: ./run.sh [test_name]
#   ./run.sh              -> list all test binaries
#   ./run.sh test_vec2    -> run a specific test
set -euo pipefail

BUILD_DIR="build"

if [[ ! -d "$BUILD_DIR" ]]; then
    echo "Build directory not found. Run ./build.sh first."
    exit 1
fi

if [[ $# -eq 0 ]]; then
    echo "Available test binaries:"
    find "$BUILD_DIR/tests" -maxdepth 1 -type f -executable -printf "  %f\n" 2>/dev/null || \
        find "$BUILD_DIR/tests" -maxdepth 1 -type f -perm +111 -exec basename {} \; 2>/dev/null
    echo ""
    echo "Usage: $0 <test_name>"
    exit 0
fi

TEST_BIN="$BUILD_DIR/tests/$1"
if [[ -x "$TEST_BIN" ]]; then
    exec "$TEST_BIN"
else
    echo "Test binary not found: $TEST_BIN"
    echo "Run ./run.sh with no arguments to list available tests."
    exit 1
fi
