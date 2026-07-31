#include "../../cuda/host/check_cuda.cuh"
#include "../../cuda/device/render.cuh"
#include "../../src/vec3.h"

#include <iostream>

/**
 * \brief First image generated with CUDA (UV color gradient).
 *
 * Build/run via repo root:
 *   source raytracer/cuda/env/longleaf_modules.sh
 *   ./run_cuda.sh color_gradient.cu
 */

int main() {
    int nx = 200;
    int ny = 100;

    int numPixels = nx * ny;
    size_t fbSize = size_t(numPixels) * sizeof(vec3);

    vec3* fb = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&fb), fbSize));

    int tx = 8;
    int ty = 8;

    dim3 blocks(nx / tx + 1, ny / ty + 1);
    dim3 threads(tx, ty);

    render<<<blocks, threads>>>(fb, nx, ny);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            size_t pixelIdx = size_t(j * nx + i);
            float r = static_cast<float>(fb[pixelIdx][0]);
            float g = static_cast<float>(fb[pixelIdx][1]);
            float b = static_cast<float>(fb[pixelIdx][2]);
            int ir = int(r * 255.99f);
            int ig = int(g * 255.99f);
            int ib = int(b * 255.99f);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    checkCudaErrors(cudaFree(fb));
    return 0;
}
