#!/usr/bin/env bash
# Source on Longleaf before building/running CUDA demos:
#   source raytracer/cuda/env/longleaf_modules.sh

if ! command -v module >/dev/null 2>&1; then
    echo "module command not found; are you on Longleaf?" >&2
    return 1 2>/dev/null || exit 1
fi

module purge
module load gcc/11.2.0 2>/dev/null || module load gcc/9.3.0 2>/dev/null || module load gcc 2>/dev/null || true
module load cmake 2>/dev/null || true

if ! module load cuda/12.2 2>/dev/null \
    && ! module load cuda/12.1 2>/dev/null \
    && ! module load cuda/11.8 2>/dev/null \
    && ! module load cuda 2>/dev/null; then
    echo "Failed to load a CUDA module. Run: module avail cuda" >&2
    return 1 2>/dev/null || exit 1
fi

# Optional: MathDx module. Otherwise CMake looks for mathdx_ROOT / sibling unpack / /opt.
module load mathdx 2>/dev/null || module load nvidia-mathdx 2>/dev/null || true

if [[ -z "${mathdx_ROOT:-}" ]]; then
    _repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
    _sib="$_repo/../nvidia-mathdx-26.06.1-cuda13/nvidia/mathdx/26.06"
    [[ -d "$_sib/include" ]] && export mathdx_ROOT="$_sib"
    unset _repo _sib
fi

echo "Loaded modules:"
module list 2>&1 || true
[[ -n "${mathdx_ROOT:-}" ]] && echo "mathdx_ROOT: $mathdx_ROOT"
command -v nvcc >/dev/null && echo "nvcc: $(nvcc --version | tail -n1)"
command -v nvidia-smi >/dev/null && nvidia-smi -L || echo "warning: no GPU visible (use a GPU OnDemand session)" >&2
