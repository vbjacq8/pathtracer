#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$ROOT_DIR/raytracer/test/cpp"
EXEC_DIR="$ROOT_DIR/raytracer/test/exec"
OUT_DIR="$ROOT_DIR/raytracer/test/out"

usage() {
    echo "Usage: $0 [source.cpp]" >&2
    echo "Builds a C++ file from raytracer/test/cpp, runs it, and writes stdout to raytracer/out." >&2
}

if [[ $# -gt 1 ]]; then
    usage
    exit 1
fi

if [[ $# -eq 1 ]]; then
    source_name="$1"
    [[ "$source_name" == *.cpp ]] || source_name="${source_name}.cpp"
    source_file="$SRC_DIR/$source_name"
else
    shopt -s nullglob
    sources=("$SRC_DIR"/*.cpp)
    shopt -u nullglob

    if [[ ${#sources[@]} -ne 1 ]]; then
        echo "Expected exactly one .cpp file in $SRC_DIR; found ${#sources[@]}." >&2
        usage
        exit 1
    fi

    source_file="${sources[0]}"
fi

if [[ ! -f "$source_file" ]]; then
    echo "Source file not found: $source_file" >&2
    exit 1
fi

mkdir -p "$EXEC_DIR" "$OUT_DIR"

base_name="$(basename "$source_file" .cpp)"
executable="$EXEC_DIR/$base_name"
output_file="$OUT_DIR/$base_name.ppm"

"${CXX:-g++}" -std=c++20 -Wall -Wextra -pedantic "$source_file" -o "$executable"
"$executable" > "$output_file"

echo "Built $executable"
echo "Wrote $output_file"
