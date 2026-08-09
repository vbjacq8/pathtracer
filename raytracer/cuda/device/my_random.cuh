#pragma once

#include <cuda_runtime.h>
#include <curanddx.hpp>
#include <cstdio>
#include <cstdlib>

/**
 * CUDA RNG plumbing (cuRANDDx). Not the public API — that lives in
 * \p my_random.h as zero-extra-arg \p randomFloat / \p randomInt / …
 *
 * This header provides:
 * - \p RNG type + \p initRandomStates for the per-pixel state table
 * - \p pathtracerDeviceRandomFloat: device bridge used by \p my_random.h
 * - \p bindDeviceRng: host publishes the table before kernel launch
 *
 * Include from CUDA TUs (e.g. \p render.cuh). Do not include from
 * \p my_random.h (avoids MathDx + name collisions in shared headers).
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
// Internal table helpers (distinct names — do not collide with my_random.h).
// ---------------------------------------------------------------------------

namespace pathtracer_cuda_rng {

__device__ inline float uniformFloat(float min, float max, RNG* states, int tid) {
    curanddx::uniform<float> dist;  // default [0, 1)
    RNG rng = states[tid];
    float u = dist.generate(rng);
    states[tid] = rng;
    return min + (max - min) * u;
}

}  // namespace pathtracer_cuda_rng

// ---------------------------------------------------------------------------
// Bridge for my_random.h: recompute pixel tid from launch indices (no shared
// mutable tid global — that would race across concurrent warps).
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
 * Call once before launching \p render.
 *
 * Must stay visible on both nvcc passes (do not gate on \p __CUDA_ARCH__ — that
 * makes the identifier disappear on the device pass and breaks host call sites).
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
