#pragma once

#include "camera.h"
#include "color.h"
#include "framebuffer.h"
#include "my_random.h"
#include <cstdint>
#include <iostream>
#include <vector>
#include "display.h"

/**
 * \brief Traces one stratified sample for a single pixel.
 * \returns Estimated radiance for that pixel sample
 */
inline vec3 samplePixel(const Camera& cam, int i, int j, int nx, int ny, Hitable* world, int depth,
                        BackgroundFn background = colorBlueWhiteGradient) {
    float u = (i + randomFloat(0.0, 1.0)) / float(nx);
    float v = (j + randomFloat(0.0, 1.0)) / float(ny);
    Ray r = cam.getRay(u, v);
    return pathTrace(r, world, depth, background);
}

/** \brief Adds one sample per pixel across the framebuffer. */
inline void renderPass(Camera& cam, Hitable* world, Framebuffer& fb, int depth,
                       BackgroundFn background = colorBlueWhiteGradient) {
    for (int j = fb.height - 1; j >= 0; j--) {
        for (int i = 0; i < fb.width; i++) {
            fb.addSample(i, j, samplePixel(cam, i, j, fb.width, fb.height, world, depth, background));
        }
    }
}

/** \brief Runs \p samplesThisFrame independent passes over the framebuffer. */
inline void renderFrame(Camera& cam, Hitable* world, Framebuffer& fb, int samplesThisFrame, int depth,
                        BackgroundFn background = colorBlueWhiteGradient) {
    for (int s = 0; s < samplesThisFrame; s++) {
        renderPass(cam, world, fb, depth, background);
    }
}

/**
 * \brief Packs tonemapped framebuffer radiance into interleaved RGB bytes (top row first)
 * laid out as [R,G,B,R,G,B], for SDL's SDL_PIXELFORMAT_RGB24
 * \sa SDL_View::present
 */
inline void framebufferToRgb(const Framebuffer& fb, std::vector<uint8_t>& rgb, const float gamma) {
    rgb.resize(fb.width * fb.height * 3);
    for (int row = 0; row < fb.height; ++row) {
        const int j = fb.height - 1 - row;
        for (int i = 0; i < fb.width; ++i) {
            vec3 display = radianceToDisplay(fb.pixel(i, j), gamma);
            const int idx = (row * fb.width + i) * 3;
            rgb[idx] = static_cast<uint8_t>(255.99 * display[0]);
            rgb[idx + 1] = static_cast<uint8_t>(255.99 * display[1]);
            rgb[idx + 2] = static_cast<uint8_t>(255.99 * display[2]);
        }
    }
}

/**
 * \brief Writes tonemapped framebuffer radiance as an ASCII PPM image.
 * \param out output stream
 */
inline void writePpm(const Framebuffer& fb, std::ostream& out, const float gamma) {
    out << "P3\n" << fb.width << " " << fb.height << "\n255\n";
    for (int j = fb.height - 1; j >= 0; j--) {
        for (int i = 0; i < fb.width; i++) {
            vec3 display = radianceToDisplay(fb.pixel(i, j), gamma);
            int ir = int(255.99 * display[0]);
            int ig = int(255.99 * display[1]);
            int ib = int(255.99 * display[2]);
            out << ir << " " << ig << " " << ib << "\n";
        }
    }
}
