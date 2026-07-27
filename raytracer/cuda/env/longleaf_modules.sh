#!/usr/bin/env bash
# Source this on Longleaf before building/running the CUDA path tracer.
#
#   source raytracer/cuda/env/longleaf_modules.sh
#
# Adjust module versions after checking:  module avail cuda
# OnDemand: start a GPU desktop / GPU Jupyter session so devices are visible.

if ! command -v module >/dev/null 2>&1; then
    echo "module command not found; are you on Longleaf?" >&2
    return 1 2>/dev/null || exit 1
fi

module purge

# Prefer explicit versions when you know what the cluster provides.
# Fallbacks keep a fresh clone working across Longleaf toolkit refreshes.
_load_ok=0
if module load gcc/11.2.0 2>/dev/null || module load gcc/9.3.0 2>/dev/null || module load gcc 2>/dev/null; then
    :
fi
if module load cmake 2>/dev/null; then
    :
fi
if module load cuda/12.2 2>/dev/null \
    || module load cuda/12.1 2>/dev/null \
    || module load cuda/11.8 2>/dev/null \
    || module load cuda 2>/dev/null; then
    _load_ok=1
fi

if [[ "$_load_ok" -ne 1 ]]; then
    echo "Failed to load a CUDA module. Run: module avail cuda" >&2
    unset _load_ok
    return 1 2>/dev/null || exit 1
fi
unset _load_ok

echo "Loaded modules:"
module list 2>&1 || true

if command -v nvcc >/dev/null 2>&1; then
    echo "nvcc: $(nvcc --version | tail -n1)"
else
    echo "warning: nvcc not on PATH after module load" >&2
fi

if command -v nvidia-smi >/dev/null 2>&1; then
    nvidia-smi -L || true
else
    echo "warning: nvidia-smi not found (start a GPU OnDemand session / srun --gres=gpu)" >&2
fi
