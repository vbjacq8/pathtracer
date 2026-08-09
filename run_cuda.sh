#!/usr/bin/env bash
# Build and run a CUDA demo from raytracer/test/cu → PPM/PNG under test/out/cuda.
#
#   source raytracer/cuda/env/longleaf_modules.sh   # Longleaf GPU session
#   ./run_cuda.sh [--rebuild] [--arch SM] [source.cu]

set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CUDA_DIR="$ROOT/raytracer/cuda"
SRC_DIR="$ROOT/raytracer/test/cu"
BUILD_DIR="$CUDA_DIR/build"
EXEC_DIR="$ROOT/raytracer/test/exec"
OUT_DIR="$ROOT/raytracer/test/out/cuda"
# Sibling of the repo: csrepos/nvidia-mathdx-.../nvidia/mathdx/26.06
MATHDX_DEFAULT="$ROOT/../nvidia-mathdx-26.06.1-cuda13/nvidia/mathdx/26.06"

rebuild=0
arch=""
src="two_spheres.cu"

usage() {
    echo "Usage: $0 [--rebuild] [--arch SM] [source.cu]" >&2
    echo "  --rebuild  Wipe cuda/build/ and reconfigure" >&2
    echo "  --arch SM  CUDA arch (e.g. 75, 80, 90); default is CMake's 80" >&2
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help) usage; exit 0 ;;
        --rebuild) rebuild=1; shift ;;
        --arch) arch="$2"; shift 2 ;;
        *.cu) src="$(basename "$1")"; shift ;;
        *)
            if [[ -f "$SRC_DIR/${1%.cu}.cu" ]]; then
                src="${1%.cu}.cu"
                shift
            else
                echo "Unknown argument: $1" >&2
                usage
                exit 1
            fi
            ;;
    esac
done

[[ -f "$SRC_DIR/$src" ]] || { echo "Not found: $SRC_DIR/$src" >&2; exit 1; }
command -v nvcc >/dev/null || { echo "nvcc not found (source longleaf_modules.sh on Longleaf)" >&2; exit 1; }
command -v cmake >/dev/null || { echo "cmake not found" >&2; exit 1; }

name="${src%.cu}"
jobs="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

[[ "$rebuild" -eq 1 ]] && rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR" "$OUT_DIR" "$EXEC_DIR"

# Point CMake at MathDx (absolute path). Needed if you only module load cuda.
if [[ -z "${mathdx_ROOT:-}" && -d "$MATHDX_DEFAULT/include" ]]; then
    mathdx_ROOT="$(cd "$MATHDX_DEFAULT" && pwd)"
fi

cmake_args=(-S "$CUDA_DIR" -B "$BUILD_DIR")
[[ -n "$arch" ]] && cmake_args+=(-DCMAKE_CUDA_ARCHITECTURES="$arch")
[[ -n "${mathdx_ROOT:-}" ]] && cmake_args+=(-Dmathdx_ROOT="$mathdx_ROOT")

cmake "${cmake_args[@]}"
cmake --build "$BUILD_DIR" --target "$name" -j"$jobs"

bin="$BUILD_DIR/bin/$name"
cp "$bin" "$EXEC_DIR/$name"

ppm="$OUT_DIR/$name.ppm"
"$EXEC_DIR/$name" > "$ppm"
python3 "$ROOT/scripts/ppm_to_png.py" "$ppm" "${ppm%.ppm}.png"

echo "Built $EXEC_DIR/$name"
echo "Wrote $ppm"
echo "Wrote ${ppm%.ppm}.png"
