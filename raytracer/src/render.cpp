#include "render.h"

#include "camera.h"
#include "renderer.h"

#include <iostream>

/**
    \brief render method that takes CLI arguments, 
 */
int render(int argc, char** argv, Hitable* world) {
    RenderOptions opts;
    switch (parseOptions(argc, argv, opts)) {
        case 0:
            break;
        case 2:
            return 0;
        default:
            return 1;
    }

    const int nx = opts.width;
    const int ny = opts.height;
    const double aspect = double(nx) / double(ny);

    Camera cam(opts.lookfrom, opts.lookat, opts.vup, opts.vfov, aspect, opts.aperture, opts.focusDist);

    Framebuffer fb(nx, ny);
    renderFrame(cam, world, fb, opts.samples, opts.depth);
    writePpm(fb, std::cout);

    return 0;
}
