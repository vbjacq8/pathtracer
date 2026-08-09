# CUDA path tracer (Longleaf / NVIDIA) — polymorphism

GPU backend lives here. Demos live under `raytracer/test/cu/`.

This branch (`cudaPolymorphism`) uses **device-new polymorphic** `Texture` /
`Material` / `Hitable` objects (virtual `hit` / `scatter` / `value`), matching
the CPU scene style via `DeviceScene` factories.

## RNG (cuRANDDx)

Two layers — keep them separate to avoid include / overload fights:

| Header | Role |
|--------|------|
| `../src/my_random.h` | **Public API only**: `randomFloat`, `randomInt`, `randomInSphere`, `randomInDisc` (same signatures on CPU and GPU) |
| `device/my_random.cuh` | **CUDA plumbing**: `RNG`, `initRandomStates`, `bindDeviceRng`, `pathtracerDeviceRandomFloat` |

CUDA demos define `PATHTRACER_CUDA_RNG`. On the device pass, `my_random.h` calls
`pathtracerDeviceRandomFloat` (implemented in `my_random.cuh`). Include
`my_random.cuh` from CUDA TUs (`render.cuh` does this **before** shared
headers). Do **not** include `.cuh` from `my_random.h`.

`SM<Arch>()` is required by the cuRANDDx descriptor API (`Arch` from
`__CUDA_ARCH__` / `PATHTRACER_CUDA_ARCH`). Only the **cuRANDDx** MathDx
component is required (`find_package(… curanddx)`).

CPU batch/interactive builds never set `PATHTRACER_CUDA_RNG`, so they keep mt19937.

## MathDx location

`run_cuda.sh` and CMake look for:

```text
<repo>/../nvidia-mathdx-26.06.1-cuda13/nvidia/mathdx/26.06/
```

(`include/curanddx.hpp` under that tree). Override with
`export mathdx_ROOT=/path/to/nvidia/mathdx/26.06` or
`cmake -Dmathdx_ROOT=...`.

MathDx 26.06 documents SM **≥ 750**. Prefer pinning the GPU:

```bash
./run_cuda.sh --rebuild --arch 80 two_spheres.cu
```

## Layout

| Path | Role |
|------|------|
| `device/` | Kernels (`render.cuh`, `path_trace.cuh`, `my_random.cuh`) |
| `host/` | Host helpers (`device_scene.cuh`, `timing.cuh`, `check_cuda.cuh`) |
| `env/longleaf_modules.sh` | `module load` helper for UNC Longleaf (CUDA + MathDx) |
| `build/` | CMake output (gitignored) |
| `../test/cu/` | CUDA demos (e.g. `two_spheres.cu`) |
| `../test/out/cuda/` | PPM/PNG from CUDA demos |

## Scene construction

Build worlds on the **host** with `DeviceScene` (`host/device_scene.cuh`):

- Textures / materials / hitables → 1-thread device factories (`new` on device for correct vtables)
- Host pools own the pointers; `buildWorld()` wraps hitables in a device `HitableList`
- `free()` tears down world → hitables → materials → textures

Demos allocate `RNG*` states, call `bindDeviceRng(states, nx)`, then
`render(..., world, states, depth)`. Each demo prints `Total runtime: <ms> ms`
to **stderr** (stdout remains PPM).

## Longleaf OnDemand

```bash
source raytracer/cuda/env/longleaf_modules.sh
./run_cuda.sh --rebuild --arch 80 two_spheres.cu
```

Compare against flat tables: `cudaFlatTable`.

## IntelliSense (local Mac, no CUDA toolkit)

`intellisense/cuda_runtime.h` stubs CUDA APIs for clangd. Real Longleaf builds use the
module toolkit headers. Kernel-launch syntax `<<<>>>` may still squiggle locally.
