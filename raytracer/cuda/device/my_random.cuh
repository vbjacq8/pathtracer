#pragma once

#include "../../src/vec3.h"

#include <cuda_runtime.h>
#include <curanddx.hpp>
#include <math_constants.h>
#include <cstdio>
#include <cstdlib>

/**
 * CUDA RNG plumbing (cuRANDDx) for flat-table kernels.
 *
 * Public zero-extra-arg API lives in \p my_random.h. This header must not
 * reuse those names with different arity — flat kernels call the namespaced
 * helpers below with an explicit \p RNG* table + tid.
 *
 * Also provides the \p pathtracerDeviceRandomFloat bridge used when shared
 * HD headers are compiled under \p PATHTRACER_CUDA_RNG.
 */

/**
 * Target SM for cuRANDDx \p SM<Arch>() (required by the descriptor API).
 * Device passes use \p __CUDA_ARCH__; host / IntelliSense fall back to
 * \p PATHTRACER_CUDA_ARCH (set by CMake when a single arch is configured).
 *
 * MathDx 26.06 documents SM >= 750 (Turing+). Prefer \p ./run_cuda.sh --arch 80
 * (etc.) to match the GPU; default 800 is Ampere.
 */
#ifndef PATHTRACER_CUDA_ARCH
#    if defined(__CUDA_ARCH__)
#        define PATHTRACER_CUDA_ARCH __CUDA_ARCH__
#    else
#        define PATHTRACER_CUDA_ARCH 800
#    endif
#endif

constexpr unsigned int Arch = PATHTRACER_CUDA_ARCH;

constexpr unsigned int SUBSEQUENCES = 4096;
constexpr unsigned long long SEED = 1234ULL;

using RNG = decltype(curanddx::Generator<curanddx::xorwow>() + curanddx::SM<Arch>() +
                     curanddx::Thread());
constexpr typename RNG::offset_type OFFSET = 2ULL;

/**
 * \brief Initialize per-thread XORWOW state (LEGACY subsequence layout).
 */
__device__ inline void initRandomStates(RNG* states, int tid, const unsigned long long seed = SEED,
                                        const typename RNG::offset_type offset = OFFSET) {
    RNG rng;
    rng.init(seed, (offset + tid) % SUBSEQUENCES, (offset + tid) / SUBSEQUENCES);
    states[tid] = rng;
}

// ---------------------------------------------------------------------------
// Flat-table helpers (distinct names — do not collide with my_random.h).
// ---------------------------------------------------------------------------

namespace pathtracer_cuda_rng {

__device__ inline float uniformFloat(float min, float max, RNG* states, int tid) {
    curanddx::uniform<float> dist;  // default [0, 1)
    RNG rng = states[tid];
    float u = dist.generate(rng);
    states[tid] = rng;
    return min + (max - min) * u;
}

__device__ inline int uniformInt(int min, int max, RNG* states, int tid) {
    float u = uniformFloat(0.0f, 1.0f, states, tid);
    int range = max - min + 1;
    int v = min + static_cast<int>(u * static_cast<float>(range));
    return v > max ? max : v;
}

__device__ inline vec3 inSphere(RNG* states, int tid) {
    float u = uniformFloat(0.0f, 1.0f, states, tid);
    float v = uniformFloat(0.0f, 1.0f, states, tid);
    float w = uniformFloat(0.0f, 1.0f, states, tid);
    float theta = 2.0f * CUDART_PI_F * u;
    float phi = acosf(2.0f * v - 1.0f);
    float r = cbrtf(w);
    float s = sinf(phi);
    return vec3(r * s * cosf(theta), r * s * sinf(theta), r * cosf(phi));
}

__device__ inline vec3 inDisc(RNG* states, int tid) {
    float u = uniformFloat(0.0f, 1.0f, states, tid);
    float v = uniformFloat(0.0f, 1.0f, states, tid);
    float theta = 2.0f * CUDART_PI_F * u;
    float r = sqrtf(v);
    return vec3(r * cosf(theta), r * sinf(theta), 0.0f);
}

}  // namespace pathtracer_cuda_rng

// ---------------------------------------------------------------------------
// Bridge for my_random.h under PATHTRACER_CUDA_RNG (shared HD headers).
// Flat kernels prefer pathtracer_cuda_rng::* with explicit states/tid.
// ---------------------------------------------------------------------------

__device__ RNG* g_pathtracer_rng_states;
__device__ int g_pathtracer_fb_width;

__device__ inline int pathtracerRngTid() {
    const int i = threadIdx.x + blockIdx.x * blockDim.x;
    const int j = threadIdx.y + blockIdx.y * blockDim.y;
    return j * g_pathtracer_fb_width + i;
}

/** \brief Device entry used by \p my_random.h when \p PATHTRACER_CUDA_RNG is set. */
__device__ inline float pathtracerDeviceRandomFloat(float min, float max) {
    return pathtracer_cuda_rng::uniformFloat(min, max, g_pathtracer_rng_states, pathtracerRngTid());
}

/**
 * \brief Host: publish RNG table + framebuffer width for the device bridge.
 * Optional on flat demos (kernels pass states explicitly); required if any
 * device code uses the zero-arg \p my_random.h API.
 *
 * Must stay visible on both nvcc passes (do not gate on \p __CUDA_ARCH__).
 */
__host__ inline void bindDeviceRng(RNG* states, int fbWidth) {
    cudaError_t err = cudaMemcpyToSymbol(g_pathtracer_rng_states, &states, sizeof(states));
    if (err != cudaSuccess) {
        fprintf(stderr, "bindDeviceRng states: %s\n", cudaGetErrorString(err));
        exit(static_cast<int>(err));
    }
    err = cudaMemcpyToSymbol(g_pathtracer_fb_width, &fbWidth, sizeof(fbWidth));
    if (err != cudaSuccess) {
        fprintf(stderr, "bindDeviceRng width: %s\n", cudaGetErrorString(err));
        exit(static_cast<int>(err));
    }
}
