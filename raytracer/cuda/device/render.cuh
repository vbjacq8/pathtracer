#pragma once

#include <cuda_runtime.h>

// RNG plumbing first so pathtracerDeviceRandomFloat is defined before
// my_random.h (via color.h) uses it on the device pass under PATHTRACER_CUDA_RNG.
#include "my_random.cuh"

#include "../../src/camera.h"
#include "../../src/color.h"
#include "../../src/ray.h"

#include "hitable_rec.cuh"
#include "material_rec.cuh"
#include "path_trace.cuh"

/**
 * \brief One thread per pixel: init RNG, jittered multi-sample path-trace.
 *
 * Samples are accumulated in-thread (same estimator as CPU \p samplePixel /
 * \p renderFrame, but without separate full-frame passes).
 */
__global__ void render(vec3* fb, int maxX, int maxY, Camera cam, int samples,
                       const HitableRec* hitables, int hitableCount, const MaterialRec* materials,
                       const TextureRec* textures, RNG* states, int maxDepth) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)) {
        return;
    }
    const int pixelIdx = j * maxX + i;

    initRandomStates(states, pixelIdx);

    vec3 col(0.0f, 0.0f, 0.0f);
    for (int s = 0; s < samples; ++s) {
        // Same jitter as CPU samplePixel; randomFloat → cuRANDDx via bindDeviceRng.
        const float u = (float(i) + randomFloat(0.0f, 1.0f)) / float(maxX);
        const float v = (float(j) + randomFloat(0.0f, 1.0f)) / float(maxY);
        const Ray r = cam.getRay(u, v);
        col += pathTrace(r, hitables, hitableCount, materials, textures, maxDepth, states,
                         pixelIdx);
    }
    fb[pixelIdx] = col / float(samples);
}

/**
 * \brief Normal-map / sky smoke kernel with optional jittered AA.
 * If \p hitables is null or count is 0, writes the blue–white sky gradient only.
 */
__global__ void renderNormals(vec3* fb, int maxX, int maxY, Camera cam, int samples,
                              const HitableRec* hitables, int hitableCount, RNG* states) {
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
        if (hitables == nullptr || hitableCount <= 0) {
            col += colorBlueWhiteGradient(r);
        } else {
            HitRecord hr;
            if (hitScene(hitables, hitableCount, r, 0.001f, infinity, hr)) {
                col += 0.5f * vec3(hr.normal.x() + 1.0f, hr.normal.y() + 1.0f, hr.normal.z() + 1.0f);
            } else {
                col += colorBlueWhiteGradient(r);
            }
        }
    }
    fb[pixelIdx] = col / float(samples);
}
