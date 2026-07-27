# CUDA path tracer (Longleaf / NVIDIA)

GPU backend lives here. It is **not** a drop-in of `raytracer/src/`; device code uses
flat structs / enums. The smoke binary verifies the toolchain after `git pull`.

## Layout

| Path | Role |
|------|------|
| `device/` | Kernels + `__device__` types (`vec3`, `ray`, material enums) |
| `host/` | Launch, CUDA error checks, PPM output |
| `env/longleaf_modules.sh` | `module load` helper for UNC Longleaf |
| `build/` | CMake output (gitignored) |

## Longleaf OnDemand

1. Start a **GPU** desktop or terminal session (CPU-only nodes have no devices).
2. Clone/pull this repo, then:

```bash
source raytracer/cuda/env/longleaf_modules.sh
./run_cuda.sh
```

Output: `raytracer/test/out/cuda_smoke.ppm` (+ `.png` if `scripts/ppm_to_png.py` runs).

Pin a GPU arch if fatbin size matters:

```bash
./run_cuda.sh --rebuild --arch 80    # A100
nvidia-smi --query-gpu=compute_cap --format=csv
```

## IntelliSense (local Mac, no CUDA toolkit)

`intellisense/cuda_runtime.h` stubs `__global__`, `blockIdx`, `cudaMalloc`, etc. so
clangd works without `nvcc`. Real Longleaf builds use the module's headers instead
(they come first on the include path from `nvcc`/`cmake`).

Reload the window or run **clangd: Restart language server** after pulling these
config changes. Kernel-launch syntax `<<<grid, block>>>` may still squiggle — that
is CUDA-only and clangd treats `.cu` as C++; it still compiles on Longleaf.
