#pragma once

#include "vec3.h"

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
    int depth = 500;
};

/** Returns 0 on success, 1 on error, 2 when --help was requested. */
int parseOptions(int argc, char** argv, RenderOptions& opts);
