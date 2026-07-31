#pragma once

#include "ray.h"
#include "vec3.h"

/**
 * \brief Command-line render settings shared by batch and interactive modes.
 */
struct RenderOptions {
    int width = 200;
    int height = 0;
    float aspect = 16.0 / 9.0;
    vec3 lookfrom{13, 2, 3};
    vec3 lookat{0, 0, 0};
    vec3 vup{0, 1, 0};
    float vfov = 20;
    float aperture = 0.0;
    float focusDist = 10.0;
    int samples = 100;
    int depth = 50;
    int displayWidth = 0;
    int displayHeight = 0;
    float gamma = 2.2;
    bool fullscreen = false;
    bool showFps = false;
    BackgroundFn background = colorBlueWhiteGradient;
};

/**
 * \brief Parses CLI options into \p opts.
 * \returns 0 on success, 1 on error, 2 when --help was requested
 */
int parseOptions(int argc, char** argv, RenderOptions& opts);
