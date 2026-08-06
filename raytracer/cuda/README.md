# CUDA path tracer (Longleaf / NVIDIA) — polymorphism

GPU backend lives here. Demos live under `raytracer/test/cu/`.

This branch (`cudaPolymorphism`) uses **device-new polymorphic** `Texture` /
`Material` / `Hitable` objects (virtual `hit` / `scatter` / `value`), matching
the CPU scene style via `DeviceScene` factories.

## Layout

| Path | Role |
|------|------|
| `device/` | Kernels (`render.cuh`, `path_trace.cuh`) |
| `host/` | Host helpers (`device_scene.cuh`, `timing.cuh`, `check_cuda.cuh`) |
| `env/longleaf_modules.sh` | `module load` helper for UNC Longleaf |
| `build/` | CMake output (gitignored) |
| `../test/cu/` | CUDA demos (e.g. `two_spheres.cu`) |
| `../test/out/cuda/` | PPM/PNG from CUDA demos |

Path tracing calls virtual `Hitable::hit` and `Material::scatter` / `emit`.
Device RNG for scatter comes from `../src/my_random.h` (hash/`clock` on device).
Shared `reflect` / `refract` / `schlick` are in `../src/optics.h`.

## Scene construction

Build worlds on the **host** with `DeviceScene` (`host/device_scene.cuh`):

- Textures / materials / hitables → 1-thread device factories (`new` on device for correct vtables)
- Host pools own the pointers; `buildWorld()` wraps hitables in a device `HitableList`
- `free()` tears down world → hitables → materials → textures

Demos call `render(..., world, depth)`. Each demo prints `Total runtime: <ms> ms`
to **stderr** (stdout remains PPM).

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

Compare against the flat-table GPU branch: `cudaFlatTable` (requires MathDx / cuRANDDx).

## IntelliSense (local Mac, no CUDA toolkit)

`intellisense/cuda_runtime.h` stubs CUDA APIs for clangd. Real Longleaf builds use the
module toolkit headers. Kernel-launch syntax `<<<>>>` may still squiggle locally.
