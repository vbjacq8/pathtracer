# pathtracer

A header-only CPU ray tracer written in C++ (no external dependencies, no services, no network). Source lives in `raytracer/src/*.h`; the runnable entry point is `raytracer/test/cpp/camera.cpp`, which renders a 200×100 scene and writes a PPM image to stdout.

## Cursor Cloud specific instructions

- This is a standalone C++ CLI program. There are **no long-running services**, databases, or ports — "running the app" means compiling the entry point and rendering an image.
- No package manager or dependency manifest exists; the only requirement is a C++ compiler (`g++` is preinstalled on the VM). Nothing needs to be installed.
- Build and run (the headers are included via relative paths, so the output binary location does not matter):
  ```
  cd raytracer
  g++ -std=c++11 -O2 test/cpp/camera.cpp -o test/exec/camera
  ./test/exec/camera > test/out/camera.ppm
  ```
- `raytracer/test/**` and `*.out` are gitignored, so build artifacts and rendered `.ppm` output are not tracked.
- Gotcha: the VM has no image viewer/`convert`/Pillow by default. To turn the `.ppm` into a viewable PNG, use a small Python stdlib script (`zlib` + `struct`) rather than relying on ImageMagick or PIL.
- There is no test framework or lint config; `camera.cpp` under `test/` is a manual render/demo, not an automated test suite.
