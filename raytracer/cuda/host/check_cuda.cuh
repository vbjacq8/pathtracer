#pragma once

#include <cuda_runtime.h>

#include <cstdlib>
#include <iostream>

inline void checkCuda(cudaError_t result, char const* const func, const char* const file,
                      int const line) {
    if (result) {
        std::cerr << "CUDA Error: " << static_cast<unsigned int>(result) << " at " << file << ":"
                  << line << " '" << func << "'\n";
        cudaDeviceReset();
        std::exit(99);
    }
}

#define checkCudaErrors(val) checkCuda((val), #val, __FILE__, __LINE__)
