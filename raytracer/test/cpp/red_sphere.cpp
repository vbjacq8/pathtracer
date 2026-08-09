#include "../../src/camera.h"
#include "../../src/hittables.h"
#include "../../src/parse.h"
#include "../../src/renderer.h"

#include <chrono>
#include <iostream>
#include <memory>

/**
 * CPU counterpart of raytracer/test/cu/red_sphere.cu: blue–white sky only
 * (empty world), classic RTIOW camera at the origin looking down -z.
 *
 *   ./run_cpp_test.sh red_sphere.cpp
 *   ./run_cpp_test.sh red_sphere.cpp --width 400 --samples 1
 */
int main(int argc, char** argv) {
    RenderOptions opts;
    opts.lookfrom = vec3(0.0f, 0.0f, 0.0f);
    opts.lookat = vec3(0.0f, 0.0f, -1.0f);
    opts.vup = vec3(0.0f, 1.0f, 0.0f);
    opts.vfov = 90.0f;
    opts.aspect = 2.0f;
    opts.width = 200;
    opts.height = 100;
    opts.aperture = 0.0f;
    opts.focusDist = 1.0f;
    opts.samples = 1;
    opts.depth = 1;
    opts.gamma = 1.0f;  // match CUDA demos (linear * 255.99)

    switch (parseOptions(argc, argv, opts)) {
        case 0:
            break;
        case 2:
            return 0;
        default:
            return 1;
    }
    opts.background = colorBlueWhiteGradient;

    auto world = std::make_shared<HitableList>();

    const float aspect = float(opts.width) / float(opts.height);
    Camera cam(opts.lookfrom, opts.lookat, opts.vup, opts.vfov, aspect, opts.aperture,
               opts.focusDist);
    Framebuffer fb(opts.width, opts.height);

    const auto t0 = std::chrono::steady_clock::now();
    renderFrame(cam, world.get(), fb, opts.samples, opts.depth, opts.background);
    const auto t1 = std::chrono::steady_clock::now();
    std::cerr << "Render time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << " ms\n";

    const auto t2 = std::chrono::steady_clock::now();
    writePpm(fb, std::cout, opts.gamma);
    const auto t3 = std::chrono::steady_clock::now();
    std::cerr << "Write time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << " ms\n";
    std::cerr << "Total: "
              << std::chrono::duration_cast<std::chrono::milliseconds>((t1 - t0) + (t3 - t2)).count()
              << " ms\n";
    return 0;
}
