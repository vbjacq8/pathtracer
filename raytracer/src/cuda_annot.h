#pragma once

/**
 * Portable CUDA call-site annotations for headers shared by g++ and nvcc.
 *
 * - Real nvcc: __CUDACC__ and __CUDACC_VER_MAJOR__ are set → keep __host__/__device__.
 * - Plain CPU compilers: HD macros expand to nothing.
 * - clangd / IDE with -D__CUDACC__ but no toolkit: stub the attributes so
 *   `__host__` is not parsed as an unknown type (which breaks class members).
 */
#if defined(__CUDACC__) && !defined(__CUDACC_VER_MAJOR__)
#ifndef __host__
#define __host__
#endif
#ifndef __device__
#define __device__
#endif
#ifndef __global__
#define __global__
#endif
#endif

#if defined(__CUDACC__)
#define PATHTRACER_HD __host__ __device__
#else
#define PATHTRACER_HD
#endif
