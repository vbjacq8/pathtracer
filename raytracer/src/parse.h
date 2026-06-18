#pragma once

#include "vec3.h"

/**
 * \brief Command-line render settings shared by batch and interactive modes.
 */
struct RenderOptions {
    int width = 200;
    int height = 0;
    double aspect = 2.0;
    vec3 lookfrom{13, 2, 3};
    vec3 lookat{0, 0, 0};
    vec3 vup{0, 1, 0};
    double vfov = 20;
    double aperture = 0.1;
    double focusDist = 10.0;
    int samples = 100;
    int depth = 50;
};

/**
 * \brief Parses CLI options into \p opts.
 * \returns 0 on success, 1 on error, 2 when --help was requested
 */
int parseOptions(int argc, char** argv, RenderOptions& opts);
