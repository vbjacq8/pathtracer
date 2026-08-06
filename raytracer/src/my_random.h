#pragma once

#include <cmath>

#include "constants.h"
#include "vec3.h"

#if !defined(__CUDA_ARCH__)
#include <random>
#endif

/**
 * Host/device random helpers.
 *
 * Selection is **compile-time**, not runtime: nvcc emits a host version and a
 * device version of each \p PATHTRACER_HD function. On the device pass,
 * \p __CUDA_ARCH__ is defined; on the host pass it is not.
 *
 * - Host (CPU batch / interactive): mt19937
 * - Device + \p PATHTRACER_CUDA_RNG (CUDA demos): cuRANDDx via \p my_random.cuh
 *   zero-arg bridge (same generator as the flat-table branch)
 * - Device without that flag: cheap hash fallback
 *
 * There is no useful runtime "am I on GPU?" switch inside one compiled body —
 * the linker already picked the host or device specialization.
 */

#if defined(__CUDA_ARCH__) && defined(PATHTRACER_CUDA_RNG)
#include "../cuda/device/my_random.cuh"
#endif

/**
 * \brief Uniform random numbers in [\p min, \p max).
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
