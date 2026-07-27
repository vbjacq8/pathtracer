#!/usr/bin/env bash
# Build and run the CUDA smoke test (gradient framebuffer → PPM).
#
# On Longleaf OnDemand (GPU session):
#   source raytracer/cuda/env/longleaf_modules.sh
#   ./run_cuda.sh
#
# Options:
#   ./run_cuda.sh --rebuild
#   ./run_cuda.sh --arch 80          # e.g. A100
#   ./run_cuda.sh -- --width 800 --out /path/to/out.ppm

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CUDA_DIR="$ROOT_DIR/raytracer/cuda"
BUILD_DIR="$CUDA_DIR/build"
OUT_DIR="$ROOT_DIR/raytracer/test/out"
PPM_TO_PNG="$ROOT_DIR/scripts/ppm_to_png.py"

rebuild=0
cuda_arch=""
program_args=()

usage() {
    echo "Usage: $0 [--rebuild] [--arch SM] [--] [cuda_smoke args...]" >&2
    echo "  --rebuild   Wipe build/ and reconfigure" >&2
    echo "  --arch SM   Pass -DCMAKE_CUDA_ARCHITECTURES=SM (e.g. 70, 80, 90)" >&2
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        --rebuild)
            rebuild=1
            shift
            ;;
        --arch)
            cuda_arch="$2"
            shift 2
            ;;
        --)
            shift
            program_args+=("$@")
            break
            ;;
        *)
            program_args+=("$1")
            shift
            ;;
    esac
done

if ! command -v nvcc >/dev/null 2>&1; then
    echo "nvcc not found. On Longleaf: source raytracer/cuda/env/longleaf_modules.sh" >&2
    exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "cmake not found. On Longleaf: module load cmake (or source longleaf_modules.sh)" >&2
    exit 1
fi

mkdir -p "$OUT_DIR"
if [[ "$rebuild" -eq 1 ]]; then
    rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"

cmake_args=(-S "$CUDA_DIR" -B "$BUILD_DIR")
if [[ -n "$cuda_arch" ]]; then
    cmake_args+=(-DCMAKE_CUDA_ARCHITECTURES="$cuda_arch")
fi

cmake "${cmake_args[@]}"
cmake --build "$BUILD_DIR" -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

ppm_path="$OUT_DIR/cuda_smoke.ppm"
"$BUILD_DIR/bin/cuda_smoke" --out "$ppm_path" "${program_args[@]}"

if [[ -f "$PPM_TO_PNG" ]]; then
    python3 "$PPM_TO_PNG" "$ppm_path" "${ppm_path%.ppm}.png"
    echo "PNG: ${ppm_path%.ppm}.png"
fi
