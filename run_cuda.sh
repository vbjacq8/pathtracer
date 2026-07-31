#!/usr/bin/env bash
# Build and run a CUDA demo from raytracer/test/cu (PPM → PNG under test/out/cuda).
#
# On Longleaf OnDemand (GPU session):
#   source raytracer/cuda/env/longleaf_modules.sh
#   ./run_cuda.sh color_gradient.cu
#
# Options:
#   ./run_cuda.sh --rebuild
#   ./run_cuda.sh --arch 80
#   ./run_cuda.sh color_gradient.cu

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CUDA_DIR="$ROOT_DIR/raytracer/cuda"
SRC_DIR="$ROOT_DIR/raytracer/test/cu"
BUILD_DIR="$CUDA_DIR/build"
EXEC_DIR="$ROOT_DIR/raytracer/test/exec"
OUT_DIR="$ROOT_DIR/raytracer/test/out/cuda"
PPM_TO_PNG="$ROOT_DIR/scripts/ppm_to_png.py"

rebuild=0
cuda_arch=""
source_file=""

usage() {
    echo "Usage: $0 [--rebuild] [--arch SM] [source.cu]" >&2
    echo "Builds a .cu demo from raytracer/test/cu, runs it, writes PPM/PNG to raytracer/test/out/cuda." >&2
    echo "  --rebuild   Wipe cuda/build/ and reconfigure" >&2
    echo "  --arch SM   Pass -DCMAKE_CUDA_ARCHITECTURES=SM (e.g. 70, 80, 90)" >&2
    echo "Example: $0 color_gradient.cu" >&2
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
        *.cu)
            if [[ -n "$source_file" ]]; then
                echo "Multiple source files specified." >&2
                usage
                exit 1
            fi
            source_file="$1"
            shift
            ;;
        *)
            if [[ -z "$source_file" && -f "$SRC_DIR/${1%.cu}.cu" ]]; then
                source_file="${1%.cu}.cu"
                shift
            else
                echo "Unknown argument: $1" >&2
                usage
                exit 1
            fi
            ;;
    esac
done

if [[ -z "$source_file" ]]; then
    source_file="color_gradient.cu"
fi
source_file="$(basename "$source_file")"
if [[ ! -f "$SRC_DIR/$source_file" ]]; then
    echo "Source file not found: $SRC_DIR/$source_file" >&2
    exit 1
fi

base_name="$(basename "$source_file" .cu)"

if ! command -v nvcc >/dev/null 2>&1; then
    echo "nvcc not found. On Longleaf: source raytracer/cuda/env/longleaf_modules.sh" >&2
    exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "cmake not found. On Longleaf: module load cmake (or source longleaf_modules.sh)" >&2
    exit 1
fi

mkdir -p "$OUT_DIR" "$EXEC_DIR"
if [[ "$rebuild" -eq 1 ]]; then
    rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"

cmake_args=(-S "$CUDA_DIR" -B "$BUILD_DIR")
if [[ -n "$cuda_arch" ]]; then
    cmake_args+=(-DCMAKE_CUDA_ARCHITECTURES="$cuda_arch")
fi

cmake "${cmake_args[@]}"
cmake --build "$BUILD_DIR" --target "$base_name" -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)"

bin_path="$BUILD_DIR/bin/$base_name"
if [[ ! -x "$bin_path" ]]; then
    echo "Build did not produce $bin_path" >&2
    exit 1
fi
cp "$bin_path" "$EXEC_DIR/$base_name"

ppm_path="$OUT_DIR/$base_name.ppm"
"$EXEC_DIR/$base_name" > "$ppm_path"

if [[ -f "$PPM_TO_PNG" ]]; then
    python3 "$PPM_TO_PNG" "$ppm_path" "${ppm_path%.ppm}.png"
    echo "PNG: ${ppm_path%.ppm}.png"
fi

echo "Built $EXEC_DIR/$base_name"
echo "Wrote $ppm_path"
