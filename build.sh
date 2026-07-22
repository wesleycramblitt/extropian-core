#!/usr/bin/env bash
# Build extropian-core (debug by default, pass -r/--release for release mode).
set -euo pipefail

BUILD_DIR="build"
BUILD_TYPE="Debug"

while [[ $# -gt 0 ]]; do
    case "$1" in
        -r|--release) BUILD_TYPE="Release"; shift ;;
        -d|--debug)   BUILD_TYPE="Debug";   shift ;;
        -c|--clean)   rm -rf "$BUILD_DIR";  shift ;;
        *) echo "Usage: $0 [-r|--release] [-d|--debug] [-c|--clean]"; exit 1 ;;
    esac
done

cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5

cmake --build "$BUILD_DIR" -j"$(nproc)"
echo "[build] done — $BUILD_TYPE build in $BUILD_DIR/"
