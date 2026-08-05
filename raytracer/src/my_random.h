#pragma once

#include <cmath>

#include "constants.h"
#include "vec3.h"

#if !defined(__CUDA_ARCH__)
#include <random>
#endif

/**
 * \brief Uniform random numbers in [\p min, \p max).
 * Host: mt19937. Device: cheap per-thread hash (replace with curand later).
 */
PATHTRACER_HD inline float randomFloat(float min, float max) {
#if defined(__CUDA_ARCH__)
    unsigned int x =
        static_cast<unsigned int>(clock()) + threadIdx.x * 374761393u +
        blockIdx.x * 668265263u + threadIdx.y * 982451653u;
    x = (x ^ (x >> 13)) * 1274126177u;
    float u = static_cast<float>(x & 0x00FFFFFFu) / static_cast<float>(0x01000000);
    return min + (max - min) * u;
#else
    static std::mt19937 engine{std::random_device{}()};
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return min + (max - min) * dist(engine);
#endif
}

inline int randomInt(int min, int max) {
    // Inclusive range [min, max], matching typical RTIOW helper usage.
    static std::mt19937 engine{std::random_device{}()};
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine);
}

/**
 * \brief Random point inside the unit sphere
 */
PATHTRACER_HD inline vec3 randomInSphere() {
    float u = randomFloat(0.0f, 1.0f);
    float v = randomFloat(0.0f, 1.0f);
    float w = randomFloat(0.0f, 1.0f);
    float theta = 2.0f * pi * u;
    float phi = acosf(2.0f * v - 1.0f);
    float r = cbrtf(w);
    float s = sinf(phi);
    return vec3(r * s * cosf(theta), r * s * sinf(theta), r * cosf(phi));
}

/**
 * \brief Random point inside the unit disc in the xy-plane.
 */
PATHTRACER_HD inline vec3 randomInDisc() {
    float u = randomFloat(0.0f, 1.0f);
    float v = randomFloat(0.0f, 1.0f);
    float theta = 2.0f * pi * u;
    float r = sqrtf(v);
    return vec3(r * cosf(theta), r * sinf(theta), 0.0f);
}
