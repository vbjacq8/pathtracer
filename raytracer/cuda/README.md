# CUDA path tracer (Longleaf / NVIDIA)

GPU backend lives here. Demos live under `raytracer/test/cu/`.

## Layout

| Path | Role |
|------|------|
| `device/` | Kernels (`render.cuh`, …) |
| `host/` | Host helpers (`check_cuda.cuh`) |
| `env/longleaf_modules.sh` | `module load` helper for UNC Longleaf |
| `build/` | CMake output (gitignored) |
| `../test/cu/` | CUDA demos (e.g. `color_gradient.cu`) |
| `../test/out/cuda/` | PPM/PNG from CUDA demos |

## Longleaf OnDemand

1. Start a **GPU** desktop or terminal session (CPU-only nodes have no devices).
2. Clone/pull this repo, then:

```bash
source raytracer/cuda/env/longleaf_modules.sh
./run_cuda.sh color_gradient.cu
```

Output: `raytracer/test/out/cuda/color_gradient.ppm` (+ `.png`).

Pin a GPU arch if needed:

```bash
./run_cuda.sh --rebuild --arch 80 color_gradient.cu    # A100
nvidia-smi --query-gpu=compute_cap --format=csv
```

## IntelliSense (local Mac, no CUDA toolkit)

`intellisense/cuda_runtime.h` stubs CUDA APIs for clangd. Real Longleaf builds use the
module toolkit headers. Kernel-launch syntax `<<<>>>` may still squiggle locally.
