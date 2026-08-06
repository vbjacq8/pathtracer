#pragma once

/**
 * Wall-clock timing helper for CUDA demos (stderr).
 * Measures host time around kernel launch + device synchronize.
 */

#include <chrono>
#include <iostream>

#include <cuda_runtime.h>

#include "check_cuda.cuh"

/**
 * \brief Runs \p launch (e.g. a kernel launch), synchronizes, prints total ms to stderr.
 * \returns Elapsed milliseconds.
 */
template <typename F>
inline float timeRenderAndReport(F&& launch, const char* label = "Total runtime") {
    const auto t0 = std::chrono::steady_clock::now();
    launch();
    checkCudaErrors(cudaDeviceSynchronize());
    const auto t1 = std::chrono::steady_clock::now();
    const float ms =
        std::chrono::duration<float, std::milli>(t1 - t0).count();
    std::cerr << label << ": " << ms << " ms\n";
    return ms;
}
