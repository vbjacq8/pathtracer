# CUDA path tracer (Longleaf / NVIDIA) — polymorphism

GPU backend lives here. Demos live under `raytracer/test/cu/`.

This branch (`cudaPolymorphism`) uses **device-new polymorphic** `Texture` /
`Material` / `Hitable` objects (virtual `hit` / `scatter` / `value`), matching
the CPU scene style via `DeviceScene` factories.

## RNG (same as `cudaFlatTable`)

CUDA demos define `PATHTRACER_CUDA_RNG`. On the **device** compile pass,
`../src/my_random.h` selects cuRANDDx through `device/my_random.cuh` (compile-time
`#if defined(__CUDA_ARCH__)`, not a runtime branch). Zero-arg `randomFloat()` /
`randomInSphere()` used by virtual `Material::scatter` read per-pixel XORWOW
state via thread indices + `bindDeviceRng` symbols.

CPU batch/interactive builds never set `PATHTRACER_CUDA_RNG`, so they keep mt19937.

## Layout

| Path | Role |
|------|------|
| `device/` | Kernels (`render.cuh`, `path_trace.cuh`, `my_random.cuh`) |
| `host/` | Host helpers (`device_scene.cuh`, `timing.cuh`, `check_cuda.cuh`) |
| `env/longleaf_modules.sh` | `module load` helper for UNC Longleaf (CUDA + MathDx) |
| `common.hpp` | `Arch` for cuRANDDx `SM<Arch>()` |
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
./run_cuda.sh two_spheres.cu
```

Compare against flat tables: `cudaFlatTable`.

## IntelliSense (local Mac, no CUDA toolkit)

`intellisense/cuda_runtime.h` stubs CUDA APIs for clangd. Real Longleaf builds use the
module toolkit headers. Kernel-launch syntax `<<<>>>` may still squiggle locally.
