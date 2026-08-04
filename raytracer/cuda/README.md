# CUDA path tracer (Longleaf / NVIDIA)

GPU backend lives here. Demos live under `raytracer/test/cu/`.

## Layout

| Path | Role |
|------|------|
| `device/` | Kernels (`render.cuh`, …) |
| `host/` | Host helpers (`check_cuda.cuh`, `device_scene.cuh`) |
| `env/longleaf_modules.sh` | `module load` helper for UNC Longleaf |
| `build/` | CMake output (gitignored) |
| `../test/cu/` | CUDA demos (e.g. `two_spheres.cu`) |
| `../test/out/cuda/` | PPM/PNG from CUDA demos |

## Scene construction

Build worlds on the **host** with `DeviceScene` (`host/device_scene.cuh`): each
`add*` launches a 1-thread device factory, pools textures/materials/hitables, and
`free()` tears everything down in a fixed order. One `.cu` file per scene — no
custom `createWorld`/`freeWorld` kernels.

## Longleaf OnDemand

1. Start a **GPU** desktop or terminal session (CPU-only nodes have no devices).
2. Clone/pull this repo, then:

```bash
source raytracer/cuda/env/longleaf_modules.sh
./run_cuda.sh two_spheres.cu
```

Output: `raytracer/test/out/cuda/two_spheres.ppm` (+ `.png`).

Pin a GPU arch if needed:

```bash
./run_cuda.sh --rebuild --arch 80 color_gradient.cu    # A100
nvidia-smi --query-gpu=compute_cap --format=csv
```

## IntelliSense (local Mac, no CUDA toolkit)

`intellisense/cuda_runtime.h` stubs CUDA APIs for clangd. Real Longleaf builds use the
module toolkit headers. Kernel-launch syntax `<<<>>>` may still squiggle locally.
