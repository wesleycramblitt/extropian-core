#!/usr/bin/env bash
# Remove the build directory and any generated artifacts.
set -euo pipefail

rm -rf build .cache compile_commands.json
echo "[clean] done"
