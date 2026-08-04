# pathtracer

A header-only CPU path tracer in C++ (no external dependencies for batch rendering). Core source lives in `raytracer/src/*.h`; batch entry points are under `raytracer/test/cpp/`.

## Cursor Cloud specific instructions

- This is a standalone C++ CLI program. There are **no long-running services**, databases, or ports — "running the app" means compiling an entry point and rendering an image.
- Batch mode needs only a C++ compiler (`g++`). Interactive mode additionally requires **SDL2** (`brew install sdl2` on macOS).
- Debug crashes: `./run_interactive.sh --debug <scene>.cpp ...` (or `./run_cpp_test.sh --debug ...`) builds with `-g` + AddressSanitizer so runtime errors print stack traces. Default builds also pass `-g` for `lldb ./raytracer/test/exec/<name>`.
- `raytracer/test/**` and `*.out` are gitignored, so build artifacts and rendered output are not tracked.
- Gotcha: the VM has no image viewer by default. Use `scripts/ppm_to_png.py` (stdlib only) to convert `.ppm` output to PNG.
- Manual render demos live under `raytracer/test/cpp/`. GoogleTest unit tests live under `raytracer/test/unit/` (`./run_unit_tests.sh`; needs `cmake` + `googletest`).

### Unit tests (GoogleTest)

```bash
brew install cmake googletest   # once, on macOS
./run_unit_tests.sh
```

Sources: `raytracer/test/unit/` (`vec3_test.cpp`, `hit_test.cpp`). Build artifacts go under `raytracer/test/unit/build/` (gitignored).

### Batch render (PPM → PNG)

```bash
./run_cpp_test.sh random_scene.cpp --width 400 --samples 50
```

Output: `raytracer/test/out/<name>.ppm` and `.png`.

### Interactive render (SDL2 window, progressive accumulation)

```bash
brew install sdl2   # once, on macOS
./run_interactive.sh interactive_camera.cpp --width 640 --height 360   # small scene, fast
./run_interactive.sh interactive_scene.cpp --width 800 --height 450    # full random scene
```

Architecture: `raytracer/src/mvc/` — model (`RenderModel`), view (`SdlView`), controller (`RenderController`).

Controls: left-drag orbit, right-drag pan, scroll dolly, WASD fly, R reset accumulation, Esc quit.

CLI options (`--lookfrom`, `--vfov`, `--depth`, etc.) are shared with batch mode via `parseOptions`.

### CUDA backend (Longleaf / NVIDIA)

Sources: `raytracer/cuda/` (`device/` kernels, `host/` launch + I/O). Not a rewire of `raytracer/src/` — flat device types, separate smoke binary.

On UNC Longleaf OnDemand (GPU session):

```bash
source raytracer/cuda/env/longleaf_modules.sh
./run_cuda.sh two_spheres.cu
```

Output: `raytracer/test/out/cuda/two_spheres.ppm`. Use `./run_cuda.sh --arch 80` (etc.) to pin SM arch. macOS/Apple Silicon cannot build/run this locally.
