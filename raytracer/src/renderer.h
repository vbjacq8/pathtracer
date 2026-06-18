#pragma once

#include "camera.h"
#include "framebuffer.h"
#include "my_random.h"
#include <cstdint>
#include <iostream>
#include <vector>

/**
 * \brief Traces one stratified sample for a single pixel.
 * \returns Estimated radiance for that pixel sample
 */
inline vec3 samplePixel(const Camera& cam, int i, int j, int nx, int ny, Hitable* world, int depth, Camera::objectColor objCol) {
    double u = (i + randomDouble(0.0, 1.0)) / double(nx);
    double v = (j + randomDouble(0.0, 1.0)) / double(ny);
    Ray r = cam.getRay(u, v);
    return objCol(r, world, depth, 0);
}

/**
 * \brief Adds one sample per pixel across the framebuffer.
 */
inline void renderPass(Camera& cam, Hitable* world, Framebuffer& fb, int depth, Camera::objectColor objCol) {
    for (int j = fb.height - 1; j >= 0; j--) {
        for (int i = 0; i < fb.width; i++) {
            fb.addSample(i, j, samplePixel(cam, i, j, fb.width, fb.height, world, depth, objCol));
        }
    }
}

/**
 * \brief Runs \p samplesThisFrame independent passes over the framebuffer.
 */
inline void renderFrame(Camera& cam, Hitable* world, Framebuffer& fb, int samplesThisFrame, int depth, Camera::objectColor objCol) {
    for (int s = 0; s < samplesThisFrame; s++) {
        renderPass(cam, world, fb, depth, objCol);
    }
}

/**
 * \brief Packs averaged framebuffer colors into interleaved RGB bytes (top row first) laid out as [R,G,B,R,G,B], for SDL's SDL_PIXELFORMAT_RGB24
 * \sa SDL_View::present
 */
inline void framebufferToRgb(const Framebuffer& fb, std::vector<uint8_t>& rgb) {
    rgb.resize(fb.width * fb.height * 3);
    for (int row = 0; row < fb.height; ++row) {
        const int j = fb.height - 1 - row;
        for (int i = 0; i < fb.width; ++i) {
            vec3 col = fb.pixel(i, j);
            const int idx = (row * fb.width + i) * 3;
            rgb[idx] = static_cast<uint8_t>(255.99 * col[0]);
            rgb[idx + 1] = static_cast<uint8_t>(255.99 * col[1]);
            rgb[idx + 2] = static_cast<uint8_t>(255.99 * col[2]);
        }
    }
}

/**
 * \brief Writes the framebuffer as an ASCII PPM image.
 * \param out output stream
 */
inline void writePpm(const Framebuffer& fb, std::ostream& out) {
    out << "P3\n" << fb.width << " " << fb.height << "\n255\n";
    for (int j = fb.height - 1; j >= 0; j--) {
        for (int i = 0; i < fb.width; i++) {
            vec3 col = fb.pixel(i, j);
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            out << ir << " " << ig << " " << ib << "\n";
        }
    }
}
