#pragma once

#include <cuda_runtime.h>

#include "../../src/ray.h"
#include "../../src/hitable.h"

/**
 * \brief Primary-ray sky gradient: one thread per pixel.
 * Direction is (viewport point - origin), matching the CPU Camera ray setup.
 */
__global__ void render(vec3* fb, int maxX, int maxY, vec3 lowerLeftCorner, vec3 horizontal,
                       vec3 vertical, vec3 origin, Hitable* dWorld) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)) {
        return;
    }
    float u = float(i) / float(maxX);
    float v = float(j) / float(maxY);
    int pixelIdx = j * maxX + i;
    vec3 direction = lowerLeftCorner + u * horizontal + v * vertical - origin;
    Ray r(origin, direction);
    //fb[pixelIdx] = colorBlueWhiteGradient(r);
    fb[pixelIdx] = colorSphere(r, vec3(0,0,-1), 0.5, vec3(1,0,0));
}
