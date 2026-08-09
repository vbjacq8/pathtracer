#pragma once

#include <cmath>

#include "constants.h"
#include "vec3.h"

/**
 * Public RNG API for materials, camera, Perlin, integrators, etc.
 *
 * One signature set everywhere — never overload these names in CUDA headers.
 *
 * Compile-time selection (nvcc host vs device pass):
 * - Host: mt19937
 * - Device + \p PATHTRACER_CUDA_RNG: cuRANDDx via \p pathtracerDeviceRandomFloat
 *   (defined in \p my_random.cuh; CUDA TUs include that header separately)
 * - Device without that flag: cheap hash fallback
 *
 * Do **not** include \p my_random.cuh from here — that pulled MathDx into every
 * consumer and collided with \p randomInt / \p randomFloat overloads.
 */

#if defined(__CUDA_ARCH__) && defined(PATHTRACER_CUDA_RNG)
__device__ float pathtracerDeviceRandomFloat(float min, float max);
#endif

#if !defined(__CUDA_ARCH__)
#include <random>
#endif

/**
 * \brief Uniform random float in [\p min, \p max).
 */
PATHTRACER_HD inline float randomFloat(float min, float max) {
#if defined(__CUDA_ARCH__)
#    if defined(PATHTRACER_CUDA_RNG)
    return pathtracerDeviceRandomFloat(min, max);
#    else
    unsigned int x = static_cast<unsigned int>(clock()) + threadIdx.x * 374761393u +
                     blockIdx.x * 668265263u + threadIdx.y * 982451653u;
    x = (x ^ (x >> 13)) * 1274126177u;
    float u = static_cast<float>(x & 0x00FFFFFFu) / static_cast<float>(0x01000000);
    return min + (max - min) * u;
#    endif
#else
    static std::mt19937 engine{std::random_device{}()};
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return min + (max - min) * dist(engine);
#endif
}

/**
 * \brief Inclusive uniform random int in [\p min, \p max].
 *
 * Implemented via \p randomFloat so the same body works on host and device
 * (including device-side Perlin construction).
 */
PATHTRACER_HD inline int randomInt(int min, int max) {
    float u = randomFloat(0.0f, 1.0f);
    int range = max - min + 1;
    int v = min + static_cast<int>(u * static_cast<float>(range));
    return v > max ? max : v;
}

/**
 * \brief Random point inside the unit sphere.
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
