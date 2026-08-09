#pragma once

#include <cuda_runtime.h>

// RNG plumbing first so pathtracerDeviceRandomFloat is defined before
// my_random.h (via color.h / path_trace.cuh) uses it on the device pass.
#include "my_random.cuh"

#include "../../src/camera.h"
#include "../../src/color.h"
#include "../../src/hitable.h"
#include "../../src/ray.h"

#include "path_trace.cuh"

/**
 * \brief One thread per pixel: init cuRANDDx, jittered multi-sample path-trace.
 *
 * Samples accumulate in-thread (same estimator as CPU \p samplePixel /
 * \p renderFrame). Pixel jitter and \p Camera::getRay use the shared
 * \p randomFloat bridge (cuRANDDx).
 */
__global__ void render(vec3* fb, int maxX, int maxY, Camera cam, int samples, Hitable* dWorld,
                       RNG* states, int maxDepth) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)) {
        return;
    }
    const int pixelIdx = j * maxX + i;

    initRandomStates(states, pixelIdx);

    vec3 col(0.0f, 0.0f, 0.0f);
    for (int s = 0; s < samples; ++s) {
        const float u = (float(i) + randomFloat(0.0f, 1.0f)) / float(maxX);
        const float v = (float(j) + randomFloat(0.0f, 1.0f)) / float(maxY);
        const Ray r = cam.getRay(u, v);
        col += pathTrace(r, dWorld, maxDepth);
    }
    fb[pixelIdx] = col / float(samples);
}

/**
 * \brief Normal-map / sky smoke kernel with optional jittered AA.
 * If \p dWorld is null, writes sky gradient only.
 */
__global__ void renderNormals(vec3* fb, int maxX, int maxY, Camera cam, int samples,
                              Hitable* dWorld, RNG* states) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)) {
        return;
    }
    const int pixelIdx = j * maxX + i;
    if (states != nullptr) {
        initRandomStates(states, pixelIdx);
    }

    vec3 col(0.0f, 0.0f, 0.0f);
    for (int s = 0; s < samples; ++s) {
        float u = float(i) / float(maxX);
        float v = float(j) / float(maxY);
        if (states != nullptr) {
            u = (float(i) + randomFloat(0.0f, 1.0f)) / float(maxX);
            v = (float(j) + randomFloat(0.0f, 1.0f)) / float(maxY);
        }
        const Ray r = cam.getRay(u, v);
        if (!dWorld) {
            col += colorBlueWhiteGradient(r);
        } else {
            col += color(r, dWorld, 1);
        }
    }
    fb[pixelIdx] = col / float(samples);
}
