#include "render.h"

#include "camera.h"
#include "renderer.h"

#include <chrono>
#include <iostream>

/**
    \brief render method that takes CLI arguments; entry point into batch render
    \copydoc renderFrame (in renderer.h)
 */
int render(int argc, char** argv, HitablePtr world, BackgroundFn background) {
    RenderOptions opts;
    switch (parseOptions(argc, argv, opts)) {
        case 0:
            break;
        case 2:
            return 0;
        default:
            return 1;
    }
    opts.background = background;

    const int nx = opts.width;
    const int ny = opts.height;
    const float aspect = float(nx) / float(ny);

    Camera cam(opts.lookfrom, opts.lookat, opts.vup, opts.vfov, aspect, opts.aperture, opts.focusDist);

    Framebuffer fb(nx, ny);

    
    const auto t0 = std::chrono::steady_clock::now();
    renderFrame(cam, world.get(), fb, opts.samples, opts.depth, opts.background);
    const auto t1 = std::chrono::steady_clock::now();
    auto renderElapsedNs = t1-t0;
    auto renderElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(renderElapsedNs).count();
    std::cerr << "Render time: " << renderElapsedMs << " ms"<< std::endl;

    const auto t2 = std::chrono::steady_clock::now();
    writePpm(fb, std::cout, opts.gamma);
    const auto t3 = std::chrono::steady_clock::now();
    auto writeElapsedNs = t3-t2;
    auto writeElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(writeElapsedNs).count();
    std::cerr << "Write time: " << writeElapsedMs << " ms"<< std::endl;
    std::cerr << "Total: " << renderElapsedMs + writeElapsedMs << " ms" << std::endl;

    return 0;
}
