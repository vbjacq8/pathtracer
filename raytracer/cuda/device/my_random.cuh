#pragma once

#include "../../src/vec3.h"

#include <cuda_runtime.h>
#include <math_constants.h>
#include <curanddx.hpp>

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

// Official cuRANDDx pattern: Generator + SM<Arch> + Thread (header-only; needs
// MathDx include dir, which also ships commonDx alongside curanddx.hpp).
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

/**
 * \brief Uniform random float in [\p min, \p max).
 */
__device__ inline float randomFloat(float min, float max, RNG* states, int tid) {
    curanddx::uniform<float> dist;  // default [0, 1)
    RNG rng = states[tid];
    float u = dist.generate(rng);
    states[tid] = rng;
    return min + (max - min) * u;
}

/**
 * \brief Inclusive uniform random int in [\p min, \p max].
 */
__device__ inline int randomInt(int min, int max, RNG* states, int tid) {
    // Map [0,1) → [min, max]; clamp so max is reachable when u is just below 1.
    float u = randomFloat(0.0f, 1.0f, states, tid);
    int range = max - min + 1;
    int v = min + static_cast<int>(u * static_cast<float>(range));
    return v > max ? max : v;
}

/**
 * \brief Random point inside the unit sphere.
 */
__device__ inline vec3 randomInSphere(RNG* states, int tid) {
    float u = randomFloat(0.0f, 1.0f, states, tid);
    float v = randomFloat(0.0f, 1.0f, states, tid);
    float w = randomFloat(0.0f, 1.0f, states, tid);
    float theta = 2.0f * CUDART_PI_F * u;
    float phi = acosf(2.0f * v - 1.0f);
    float r = cbrtf(w);
    float s = sinf(phi);
    return vec3(r * s * cosf(theta), r * s * sinf(theta), r * cosf(phi));
}

/**
 * \brief Random point inside the unit disc in the xy-plane.
 */
__device__ inline vec3 randomInDisc(RNG* states, int tid) {
    float u = randomFloat(0.0f, 1.0f, states, tid);
    float v = randomFloat(0.0f, 1.0f, states, tid);
    float theta = 2.0f * CUDART_PI_F * u;
    float r = sqrtf(v);
    return vec3(r * cosf(theta), r * sinf(theta), 0.0f);
}

// ---------------------------------------------------------------------------
// Zero-arg bridge for polymorphic Material::scatter (same API as my_random.h).
// Each thread recomputes its pixel tid from block/thread indices — no shared
// mutable tid global (that would race across concurrent warps).
// ---------------------------------------------------------------------------

#include <cstdio>
#include <cstdlib>

__device__ RNG* g_pathtracer_rng_states;
__device__ int g_pathtracer_fb_width;

__device__ inline int pathtracerRngTid() {
    const int i = threadIdx.x + blockIdx.x * blockDim.x;
    const int j = threadIdx.y + blockIdx.y * blockDim.y;
    return j * g_pathtracer_fb_width + i;
}

/** \brief Device entry used by \p my_random.h when \p PATHTRACER_CUDA_RNG is set. */
__device__ inline float pathtracerDeviceRandomFloat(float min, float max) {
    return randomFloat(min, max, g_pathtracer_rng_states, pathtracerRngTid());
}

/**
 * \brief Host: publish RNG table + framebuffer width for the zero-arg device bridge.
 * Call once before launching \p render.
 */
#if !defined(__CUDA_ARCH__)
inline void bindDeviceRng(RNG* states, int fbWidth) {
    cudaError_t err = cudaMemcpyToSymbol(g_pathtracer_rng_states, &states, sizeof(states));
    if (err != cudaSuccess) {
        std::fprintf(stderr, "bindDeviceRng states: %s\n", cudaGetErrorString(err));
        std::exit(static_cast<int>(err));
    }
    err = cudaMemcpyToSymbol(g_pathtracer_fb_width, &fbWidth, sizeof(fbWidth));
    if (err != cudaSuccess) {
        std::fprintf(stderr, "bindDeviceRng width: %s\n", cudaGetErrorString(err));
        std::exit(static_cast<int>(err));
    }
}
#endif
