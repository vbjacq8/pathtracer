#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
UNIT_DIR="$ROOT_DIR/raytracer/test/unit"
BUILD_DIR="$UNIT_DIR/build"

usage() {
    echo "Usage: $0 [--clean]" >&2
    echo "Configure, build, and run GoogleTest unit tests under raytracer/test/unit." >&2
    echo "Requires: cmake, a C++20 compiler, and googletest (brew install cmake googletest)." >&2
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
    usage
    exit 0
fi

if [[ "${1:-}" == "--clean" ]]; then
    rm -rf "$BUILD_DIR"
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "cmake not found. Install with: brew install cmake" >&2
    exit 1
fi

mkdir -p "$BUILD_DIR"
cmake -S "$UNIT_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"
"$BUILD_DIR/pathtracer_unit_tests"
