# pathtracer

A header-only CPU path tracer in C++ (no external dependencies for batch rendering). Core source lives in `raytracer/src/*.h`; batch entry points are under `raytracer/test/cpp/`.

## Cursor Cloud specific instructions

- This is a standalone C++ CLI program. There are **no long-running services**, databases, or ports — "running the app" means compiling an entry point and rendering an image.
- Batch mode needs only a C++ compiler (`g++`). Interactive mode additionally requires **SDL2** (`brew install sdl2` on macOS).
- `raytracer/test/**` and `*.out` are gitignored, so build artifacts and rendered output are not tracked.
- Gotcha: the VM has no image viewer by default. Use `scripts/ppm_to_png.py` (stdlib only) to convert `.ppm` output to PNG.
- There is no test framework; files under `raytracer/test/cpp/` are manual demos.

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
