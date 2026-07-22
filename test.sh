#!/usr/bin/env bash
# Build (debug) and run the test suite.
set -euo pipefail

BUILD_DIR="build"

cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5

cmake --build "$BUILD_DIR" -j"$(nproc)"

ctest --test-dir "$BUILD_DIR" -j"$(nproc)" --output-on-failure "$@"
