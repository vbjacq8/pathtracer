#pragma once

#include "../../src/camera.h"
#include "../../src/parse.h"

#include <iostream>

/**
 * \brief Classic RTIOW demo camera defaults (origin looking down -z, 200x100).
 */
inline void setRtiowDemoDefaults(RenderOptions& opts, int depth = 50) {
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
    opts.depth = depth;
    opts.gamma = 1.0f;  // linear * 255.99 (matches existing CUDA smoke output)
}

/**
 * \brief Runs shared \p parseOptions; returns 0 ok, 2 help, 1 error.
 */
inline int applyCudaCli(int argc, char** argv, RenderOptions& opts) {
    switch (parseOptions(argc, argv, opts)) {
        case 0:
            return 0;
        case 2:
            return 2;
        default:
            return 1;
    }
}

/** \brief Builds a thin-lens / pinhole camera from parsed render options. */
inline Camera makeCameraFromOpts(const RenderOptions& opts) {
    const float aspect = float(opts.width) / float(opts.height);
    return Camera(opts.lookfrom, opts.lookat, opts.vup, opts.vfov, aspect, opts.aperture,
                  opts.focusDist);
}

/** \brief Writes an ASCII PPM with linear encode (same as prior CUDA demos). */
inline void writeCudaPpm(const vec3* fb, int nx, int ny, std::ostream& out) {
    out << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            const vec3& p = fb[j * nx + i];
            out << int(p[0] * 255.99f) << " " << int(p[1] * 255.99f) << " "
                << int(p[2] * 255.99f) << "\n";
        }
    }
}
