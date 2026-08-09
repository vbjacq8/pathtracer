#include "../../cuda/host/batch_cli.cuh"
#include "../../cuda/host/check_cuda.cuh"
#include "../../cuda/host/timing.cuh"
#include "../../cuda/device/render.cuh"

#include <iostream>

/**
 * \brief Blue-white sky gradient via CUDA (RTIOW early camera setup).
 *
 * Build/run via repo root:
 *   source raytracer/cuda/env/longleaf_modules.sh
 *   ./run_cuda.sh --rebuild red_sphere.cu
 *   ./run_cuda.sh red_sphere.cu --width 400 --samples 1
 *
 * Total render runtime is printed to stderr for Longleaf benchmarking.
 */
int main(int argc, char** argv) {
    RenderOptions opts;
    setRtiowDemoDefaults(opts, /*depth=*/1);
    const int cli = applyCudaCli(argc, argv, opts);
    if (cli == 2) {
        return 0;
    }
    if (cli != 0) {
        return 1;
    }

    const int nx = opts.width;
    const int ny = opts.height;
    const int numPixels = nx * ny;
    const Camera cam = makeCameraFromOpts(opts);

    vec3* fb = nullptr;
    checkCudaErrors(
        cudaMallocManaged(reinterpret_cast<void**>(&fb), size_t(numPixels) * sizeof(vec3)));

    RNG* states = nullptr;
    checkCudaErrors(
        cudaMallocManaged(reinterpret_cast<void**>(&states), size_t(numPixels) * sizeof(RNG)));
    bindDeviceRng(states, nx);

    dim3 blocks(nx / 8 + 1, ny / 8 + 1);
    dim3 threads(8, 8);

    timeRenderAndReport([&]() {
        renderNormals<<<blocks, threads>>>(fb, nx, ny, cam, opts.samples, nullptr, 0, states);
        checkCudaErrors(cudaGetLastError());
    });

    writeCudaPpm(fb, nx, ny, std::cout);

    checkCudaErrors(cudaFree(states));
    checkCudaErrors(cudaFree(fb));
    return 0;
}
