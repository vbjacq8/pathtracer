#pragma once

/**
 * Minimal CUDA runtime / builtin stubs for IntelliSense on machines without a
 * CUDA toolkit (e.g. Apple Silicon). Real nvcc builds ignore this directory
 * (CMake does not add -Iintellisense).
 */

#include <cstddef>
#include <cstdlib>

#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif
#ifndef __global__
#define __global__
#endif
#ifndef __shared__
#define __shared__
#endif
#ifndef __constant__
#define __constant__
#endif
#ifndef __forceinline__
#define __forceinline__ inline
#endif
#ifndef __restrict__
#define __restrict__
#endif

struct dim3 {
    unsigned int x, y, z;
    dim3(unsigned int x = 1, unsigned int y = 1, unsigned int z = 1) : x(x), y(y), z(z) {}
};

struct uint3 {
    unsigned int x, y, z;
};

struct float3 {
    float x, y, z;
};

using cudaError_t = int;
enum {
    cudaSuccess = 0,
    cudaErrorInvalidValue = 1,
};

enum cudaMemcpyKind {
    cudaMemcpyHostToHost = 0,
    cudaMemcpyHostToDevice = 1,
    cudaMemcpyDeviceToHost = 2,
    cudaMemcpyDeviceToDevice = 3,
    cudaMemcpyDefault = 4,
};

struct cudaDeviceProp {
    char name[256];
    size_t totalGlobalMem;
    int major;
    int minor;
};

inline const char* cudaGetErrorString(cudaError_t) { return "stub"; }

inline cudaError_t cudaGetDeviceCount(int* count) {
    if (count) {
        *count = 0;
    }
    return cudaSuccess;
}

inline cudaError_t cudaGetDeviceProperties(cudaDeviceProp*, int) { return cudaSuccess; }
inline cudaError_t cudaMalloc(void**, size_t) { return cudaSuccess; }
inline cudaError_t cudaMallocManaged(void**, size_t, unsigned int = 0) { return cudaSuccess; }
inline cudaError_t cudaFree(void*) { return cudaSuccess; }
inline cudaError_t cudaMemcpy(void*, const void*, size_t, cudaMemcpyKind) { return cudaSuccess; }
inline cudaError_t cudaMemset(void*, int, size_t) { return cudaSuccess; }
inline cudaError_t cudaDeviceSynchronize() { return cudaSuccess; }
inline cudaError_t cudaGetLastError() { return cudaSuccess; }
inline cudaError_t cudaDeviceReset() { return cudaSuccess; }

// Older clang CUDA lowering for <<<>>>
inline cudaError_t cudaConfigureCall(dim3, dim3, size_t = 0, void* = nullptr) { return cudaSuccess; }
inline cudaError_t cudaSetupArgument(const void*, size_t, size_t) { return cudaSuccess; }
inline cudaError_t cudaLaunch(const void*) { return cudaSuccess; }

// Newer clang CUDA lowering for <<<>>>
inline unsigned __cudaPushCallConfiguration(dim3, dim3, size_t = 0, void* = nullptr) { return 0; }
inline cudaError_t __cudaPopCallConfiguration(dim3*, dim3*, size_t*, void*) { return cudaSuccess; }

static const uint3 threadIdx{0, 0, 0};
static const uint3 blockIdx{0, 0, 0};
static const dim3 blockDim{1, 1, 1};
static const dim3 gridDim{1, 1, 1};
static const int warpSize = 32;
