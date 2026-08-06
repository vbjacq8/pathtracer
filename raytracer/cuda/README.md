# CUDA path tracer (Longleaf / NVIDIA) — flat tables

GPU backend lives here. Demos live under `raytracer/test/cu/`.

This branch (`cudaFlatTable`) uses **flat POD tables** for textures, materials, and
hitables — no device `new` / virtual dispatch.

## Layout

| Path | Role |
|------|------|
| `device/` | Kernels + flat records (`hitable_rec.cuh`, `material_rec.cuh`, …) |
| `host/` | Host helpers (`device_scene.cuh`, `timing.cuh`, `check_cuda.cuh`) |
| `env/longleaf_modules.sh` | `module load` helper for UNC Longleaf (CUDA + MathDx) |
| `common.hpp` | `Arch` for cuRANDDx `SM<Arch>()` |
| `build/` | CMake output (gitignored) |
| `../test/cu/` | CUDA demos (e.g. `two_spheres.cu`) |
| `../test/out/cuda/` | PPM/PNG from CUDA demos |

Device RNG (`device/my_random.cuh`) uses **cuRANDDx** (MathDx). Flat dispatch:
`hitScene` / `scatterMaterial` switch on `HitableType` / `MatType`. Shared
`reflect` / `refract` / `schlick` are in `../src/optics.h`.

On Longleaf, `longleaf_modules.sh` loads MathDx when available and exports `mathdx_ROOT` for
CMake (`find_package(mathdx … curanddx)`). Override with
`export mathdx_ROOT=/path/to/nvidia/mathdx/YY.MM` if the module name differs.

## Scene construction

Build worlds on the **host** with `DeviceScene` (`host/device_scene.cuh`):

- Textures / materials / hitables → flat host vectors (`TextureRec`, `MaterialRec`, `HitableRec`)
- `buildWorld()` uploads managed tables once
- Supported hitables: static sphere, moving sphere, quad

Demos call `render(..., hitables, count, materials, textures, states, depth)`.
Each demo prints `Total runtime: <ms> ms` to **stderr** (stdout remains PPM).

## Longleaf OnDemand

1. Start a **GPU** desktop or terminal session (CPU-only nodes have no devices).
2. Clone/pull this repo, then:

```bash
source raytracer/cuda/env/longleaf_modules.sh
./run_cuda.sh two_spheres.cu
# runtime is on stderr; PPM is captured to raytracer/test/out/cuda/
```

Pin a GPU arch if needed:

```bash
./run_cuda.sh --rebuild --arch 80 two_spheres.cu    # A100
nvidia-smi --query-gpu=compute_cap --format=csv
```

Compare against the polymorphic GPU branch: `cudaPolymorphism`.

## IntelliSense (local Mac, no CUDA toolkit)

`intellisense/cuda_runtime.h` stubs CUDA APIs for clangd. Real Longleaf builds use the
module toolkit headers. Kernel-launch syntax `<<<>>>` may still squiggle locally.
