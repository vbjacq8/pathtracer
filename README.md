# pathtracer

CPU path tracer in C++ with progressive accumulation and an optional SDL2 interactive viewer.

## Batch render

Renders to PPM and converts to PNG:

```bash
./run_cpp_test.sh random_scene.cpp --width 400 --samples 50
```

## Interactive render

Opens an SDL window; the image refines as samples accumulate. Requires SDL2:

```bash
brew install sdl2
./run_interactive.sh interactive_camera.cpp --width 640 --height 360
```

Use `interactive_scene.cpp` for the full random scene. Camera controls: orbit (left drag), pan (right drag), dolly (scroll), fly (WASD), reset (R), quit (Esc).

## CUDA (Longleaf / NVIDIA)

```bash
source raytracer/cuda/env/longleaf_modules.sh   # OnDemand GPU session
./run_cuda.sh
```

See `raytracer/cuda/README.md`.

## Layout

- `raytracer/src/` — tracer headers (camera, materials, framebuffer, renderer)
- `raytracer/src/mvc/` — interactive model/view/controller layer
- `raytracer/cuda/` — GPU backend skeleton (device kernels + host launch)
- `raytracer/test/cpp/` — demo entry points
- `run_cpp_test.sh` / `run_interactive.sh` / `run_cuda.sh` — build and run helpers
