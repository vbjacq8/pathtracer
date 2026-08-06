#include "../../cuda/host/check_cuda.cuh"
#include "../../cuda/host/timing.cuh"
#include "../../cuda/device/render.cuh"
#include "../../src/vec3.h"

#include <iostream>

/**
 * \brief Blue-white sky gradient via CUDA (RTIOW early camera setup).
 *
 * Build/run via repo root:
 *   source raytracer/cuda/env/longleaf_modules.sh
 *   ./run_cuda.sh --rebuild red_sphere.cu
 *
 * Total render runtime is printed to stderr for Longleaf benchmarking.
 */

int main() {
    int nx = 200;
    int ny = 100;

    // Classic RTIOW viewport (camera at origin looking down -z).
    vec3 lowerLeftCorner(-2.0f, -1.0f, -1.0f);
    vec3 horizontal(4.0f, 0.0f, 0.0f);
    vec3 vertical(0.0f, 2.0f, 0.0f);
    vec3 origin(0.0f, 0.0f, 0.0f);

    int numPixels = nx * ny;
    size_t fbSize = size_t(numPixels) * sizeof(vec3);

    vec3* fb = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&fb), fbSize));

    int tx = 8;
    int ty = 8;
    dim3 blocks(nx / tx + 1, ny / ty + 1);
    dim3 threads(tx, ty);

    timeRenderAndReport([&]() {
        renderNormals<<<blocks, threads>>>(fb, nx, ny, lowerLeftCorner, horizontal, vertical,
                                           origin, nullptr);
        checkCudaErrors(cudaGetLastError());
    });

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            size_t pixelIdx = size_t(j * nx + i);
            float r = fb[pixelIdx][0];
            float g = fb[pixelIdx][1];
            float b = fb[pixelIdx][2];
            int ir = int(r * 255.99f);
            int ig = int(g * 255.99f);
            int ib = int(b * 255.99f);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    checkCudaErrors(cudaFree(fb));
    return 0;
}
