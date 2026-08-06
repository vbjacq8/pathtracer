#pragma once

#include <cuda_runtime.h>

#include "../../src/color.h"
#include "../../src/ray.h"

#include "hitable_rec.cuh"
#include "material_rec.cuh"
#include "my_random.cuh"
#include "path_trace.cuh"

/**
 * \brief One thread per pixel: init RNG, path-trace with flat tables.
 */
__global__ void render(vec3* fb, int maxX, int maxY, vec3 lowerLeftCorner, vec3 horizontal,
                       vec3 vertical, vec3 origin, const HitableRec* hitables, int hitableCount,
                       const MaterialRec* materials, const TextureRec* textures, RNG* states,
                       int maxDepth) {
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

    fb[pixelIdx] =
        pathTrace(r, hitables, hitableCount, materials, textures, maxDepth, states, pixelIdx);
}

/**
 * \brief Normal-map / sky smoke kernel (no RNG / material tables).
 * If \p hitables is null or count is 0, writes the blue–white sky gradient only.
 */
__global__ void renderNormals(vec3* fb, int maxX, int maxY, vec3 lowerLeftCorner, vec3 horizontal,
                              vec3 vertical, vec3 origin, const HitableRec* hitables,
                              int hitableCount) {
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
    if (hitables == nullptr || hitableCount <= 0) {
        fb[pixelIdx] = colorBlueWhiteGradient(r);
        return;
    }
    HitRecord hr;
    if (hitScene(hitables, hitableCount, r, 0.001f, infinity, hr)) {
        fb[pixelIdx] = 0.5f * vec3(hr.normal.x() + 1.0f, hr.normal.y() + 1.0f, hr.normal.z() + 1.0f);
    } else {
        fb[pixelIdx] = colorBlueWhiteGradient(r);
    }
}
