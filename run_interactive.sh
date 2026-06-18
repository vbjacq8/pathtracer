#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$ROOT_DIR/raytracer/test/cpp"
EXEC_DIR="$ROOT_DIR/raytracer/test/exec"
LIB_SRC_DIR="$ROOT_DIR/raytracer/src"
MVC_DIR="$LIB_SRC_DIR/mvc"

usage() {
    echo "Usage: $0 [source.cpp] [program args...]" >&2
    echo "Builds an interactive SDL viewer from raytracer/test/cpp and runs it." >&2
    echo "Requires SDL2 (brew install sdl2 on macOS)." >&2
    echo "Example: $0 interactive_scene.cpp --width 800 --height 450" >&2
    echo "" >&2
    echo "Controls: left-drag orbit, right-drag pan, scroll dolly, WASD fly, R reset, Esc quit" >&2
}

sdl_cflags() {
    if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists sdl2; then
        pkg-config --cflags sdl2
        return 0
    fi
    if command -v sdl2-config >/dev/null 2>&1; then
        sdl2-config --cflags
        return 0
    fi
    for prefix in /opt/homebrew /usr/local; do
        if [[ -f "$prefix/include/SDL2/SDL.h" ]]; then
            echo "-I$prefix/include/SDL2 -D_THREAD_SAFE"
            return 0
        fi
    done
    return 1
}

sdl_libs() {
    if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists sdl2; then
        pkg-config --libs sdl2
        return 0
    fi
    if command -v sdl2-config >/dev/null 2>&1; then
        sdl2-config --libs
        return 0
    fi
    for prefix in /opt/homebrew /usr/local; do
        if [[ -f "$prefix/lib/libSDL2.dylib" || -f "$prefix/lib/libSDL2.so" ]]; then
            echo "-L$prefix/lib -lSDL2"
            return 0
        fi
    done
    return 1
}

source_file=""
program_args=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
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
    source_file="interactive_scene.cpp"
fi

[[ "$source_file" == *.cpp ]] || source_file="${source_file%.cpp}.cpp"
source_file="$SRC_DIR/$(basename "$source_file")"

if [[ ! -f "$source_file" ]]; then
    echo "Source file not found: $source_file" >&2
    exit 1
fi

if ! SDL_CFLAGS="$(sdl_cflags)"; then
    echo "SDL2 not found. Install with: brew install sdl2" >&2
    exit 1
fi
if ! SDL_LIBS="$(sdl_libs)"; then
    echo "SDL2 libraries not found. Install with: brew install sdl2" >&2
    exit 1
fi

mkdir -p "$EXEC_DIR"

base_name="$(basename "$source_file" .cpp)"
executable="$EXEC_DIR/$base_name"

lib_sources=(
    "$LIB_SRC_DIR/parse.cpp"
    "$LIB_SRC_DIR/render.cpp"
    "$MVC_DIR/interactive.cpp"
    "$MVC_DIR/sdl_view.cpp"
)

# shellcheck disable=SC2086
"${CXX:-g++}" -std=c++20 -O2 -Wall -Wextra -pedantic -I"$LIB_SRC_DIR" $SDL_CFLAGS \
    "$source_file" "${lib_sources[@]}" $SDL_LIBS -o "$executable"

for arg in ${program_args[@]+"${program_args[@]}"}; do
    if [[ "$arg" == "--help" || "$arg" == "-?" ]]; then
        "$executable" ${program_args[@]+"${program_args[@]}"}
        exit 0
    fi
done

echo "Built $executable"
echo "Controls: left-drag orbit, right-drag pan, scroll dolly, WASD fly, R reset, Esc quit"
"$executable" ${program_args[@]+"${program_args[@]}"}
