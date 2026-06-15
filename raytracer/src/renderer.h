#pragma once

#include "camera.h"
#include "framebuffer.h"
#include "my_random.h"
#include <iostream>

/**
*\brief connects the viewport to the world by shooting rays
*\param cam Camera object to be used
*\param i horizontal index
*\param j vertical index
*\param nx total pixels in x
*\param ny total pixels in y
*\param world collection of Hitable objects to be considered
*\param depth max number of collisions
*\param objCol strategy used for coloring
*\returns color vector that is added to a framebuffer pixel \relates renderPass
*/

inline vec3 samplePixel(const Camera& cam, int i, int j, int nx, int ny, Hitable* world, int depth, Camera::objectColor objCol) {
    double u = (i + randomDouble(0.0, 1.0)) / double(nx);
    double v = (j + randomDouble(0.0, 1.0)) / double(ny);
    Ray r = cam.getRay(u, v);
    return objCol(r, world, depth, 0);
}

/**
*\brief writes color samples to a framebuffer entry
*\param fb framebuffer that is written to
*\relates samplePixel
*/
inline void renderPass(Camera& cam, Hitable* world, Framebuffer& fb, int depth, Camera::objectColor objCol) {
    for (int j = fb.height - 1; j >= 0; j--) {
        for (int i = 0; i < fb.width; i++) {
            fb.addSample(i, j, samplePixel(cam, i, j, fb.width, fb.height, world, depth, objCol));
        }
    }
}

/**
*\brief writes color samples across the whole framebuffer
*\param samplesThisFrame number of passes through the framebuffer
*/
inline void render(Camera& cam, Hitable* world, Framebuffer& fb, int samplesThisFrame, int depth, Camera::objectColor objCol) {
    for (int s = 0; s < samplesThisFrame; s++) {
        renderPass(cam, world, fb, depth, objCol);
    }
}

/**
*\brief takes the color vectors stored in the framebuffer and writes to output stream in RGB format
*\param out ostream to write to
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
