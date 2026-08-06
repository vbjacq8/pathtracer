#pragma once

#include <cuda_runtime.h>

#include "../../src/color.h"
#include "../../src/hitable.h"
#include "../../src/ray.h"

#include "my_random.cuh"
#include "path_trace.cuh"

/**
 * \brief One thread per pixel: init cuRANDDx, path-trace with polymorphic materials.
 */
__global__ void render(vec3* fb, int maxX, int maxY, vec3 lowerLeftCorner, vec3 horizontal,
                       vec3 vertical, vec3 origin, Hitable* dWorld, RNG* states, int maxDepth) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)) {
        return;
    }
    const int pixelIdx = j * maxX + i;

    initRandomStates(states, pixelIdx);

    const float u = float(i) / float(maxX);
    const float v = float(j) / float(maxY);
    const vec3 direction = lowerLeftCorner + u * horizontal + v * vertical - origin;
    const Ray r(origin, direction);

    fb[pixelIdx] = pathTrace(r, dWorld, maxDepth);
}

/**
 * \brief Normal-map smoke kernel. If \p dWorld is null, writes sky gradient only.
 */
__global__ void renderNormals(vec3* fb, int maxX, int maxY, vec3 lowerLeftCorner, vec3 horizontal,
                              vec3 vertical, vec3 origin, Hitable* dWorld) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)) {
        return;
    }
    const float u = float(i) / float(maxX);
    const float v = float(j) / float(maxY);
    const int pixelIdx = j * maxX + i;
    const vec3 direction = lowerLeftCorner + u * horizontal + v * vertical - origin;
    const Ray r(origin, direction);
    if (!dWorld) {
        fb[pixelIdx] = colorBlueWhiteGradient(r);
        return;
    }
    fb[pixelIdx] = color(r, dWorld, 1);
}
