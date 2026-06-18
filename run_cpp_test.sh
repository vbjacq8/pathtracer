#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$ROOT_DIR/raytracer/test/cpp"
EXEC_DIR="$ROOT_DIR/raytracer/test/exec"
OUT_DIR="$ROOT_DIR/raytracer/test/out"
PPM_TO_PNG="$ROOT_DIR/scripts/ppm_to_png.py"

usage() {
    echo "Usage: $0 [source.cpp] [program args...]" >&2
    echo "Builds a C++ file from raytracer/test/cpp, runs it, writes PPM to raytracer/test/out," >&2
    echo "and converts the result to PNG." >&2
    echo "Example: $0 random_scene.cpp --samples 50 --width 400" >&2
}

source_file=""
program_args=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            if [[ -z "$source_file" && $# -eq 1 ]]; then
                usage
                exit 0
            fi
            program_args+=("$1")
            shift
            ;;
        --)
            shift
            program_args+=("$@")
            break
            ;;
        *.cpp)
            if [[ -n "$source_file" ]]; then
                echo "Multiple source files specified." >&2
                usage
                exit 1
            fi
            source_file="$1"
            shift
            ;;
        *)
            if [[ -z "$source_file" && -f "$SRC_DIR/${1%.cpp}.cpp" ]]; then
                source_file="${1%.cpp}.cpp"
                shift
            else
                program_args+=("$1")
                shift
            fi
            ;;
    esac
done

if [[ -z "$source_file" ]]; then
    shopt -s nullglob
    sources=("$SRC_DIR"/*.cpp)
    shopt -u nullglob

    if [[ ${#sources[@]} -ne 1 ]]; then
        echo "Expected exactly one .cpp file in $SRC_DIR; found ${#sources[@]}." >&2
        usage
        exit 1
    fi

    source_file="$(basename "${sources[0]}")"
fi

[[ "$source_file" == *.cpp ]] || source_file="${source_file%.cpp}.cpp"
source_file="$SRC_DIR/$(basename "$source_file")"

if [[ ! -f "$source_file" ]]; then
    echo "Source file not found: $source_file" >&2
    exit 1
fi

mkdir -p "$EXEC_DIR" "$OUT_DIR"

base_name="$(basename "$source_file" .cpp)"
executable="$EXEC_DIR/$base_name"
output_file="$OUT_DIR/$base_name.ppm"
png_file="$OUT_DIR/$base_name.png"

LIB_SRC_DIR="$ROOT_DIR/raytracer/src"
lib_sources=()
for lib_file in parse.cpp render.cpp; do
    if [[ -f "$LIB_SRC_DIR/$lib_file" ]]; then
        lib_sources+=("$LIB_SRC_DIR/$lib_file")
    fi
done

"${CXX:-g++}" -std=c++20 -O2 -Wall -Wextra -pedantic -I"$LIB_SRC_DIR" "$source_file" "${lib_sources[@]}" -o "$executable"

for arg in ${program_args[@]+"${program_args[@]}"}; do
    if [[ "$arg" == "--help" || "$arg" == "-?" ]]; then
        "$executable" ${program_args[@]+"${program_args[@]}"}
        exit 0
    fi
done

"$executable" ${program_args[@]+"${program_args[@]}"} > "$output_file"
python3 "$PPM_TO_PNG" "$output_file" > /dev/null

echo "Built $executable"
echo "Wrote $output_file"
echo "Wrote $png_file"
