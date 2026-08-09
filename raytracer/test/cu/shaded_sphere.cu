#include "../../cuda/host/batch_cli.cuh"
#include "../../cuda/host/check_cuda.cuh"
#include "../../cuda/host/device_scene.cuh"
#include "../../cuda/host/timing.cuh"
#include "../../cuda/device/render.cuh"

#include <iostream>

/**
 * Minimal polymorphic DeviceScene path-trace smoke test: one sphere + ground.
 * RNG: my_random.h public API + my_random.cuh cuRANDDx bridge.
 *
 *   ./run_cuda.sh --rebuild shaded_sphere.cu
 *   ./run_cuda.sh shaded_sphere.cu --width 400 --samples 50 --depth 10
 *
 * Total render runtime is printed to stderr for Longleaf benchmarking.
 */
int main(int argc, char** argv) {
    RenderOptions opts;
    setRtiowDemoDefaults(opts, /*depth=*/10);
    const int cli = applyCudaCli(argc, argv, opts);
    if (cli == 2) {
        return 0;
    }
    if (cli != 0) {
        return 1;
    }

    DeviceScene scene;
    Material* red = scene.addLambertian(vec3(1.0f, 0.0f, 0.0f));
    Material* ground = scene.addLambertian(vec3(0.8f, 0.8f, 0.0f));
    scene.addSphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, red);
    scene.addSphere(vec3(0.0f, -100.5f, -1.0f), 100.0f, ground);
    Hitable* world = scene.buildWorld();

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
        render<<<blocks, threads>>>(fb, nx, ny, cam, opts.samples, world, states, opts.depth);
        checkCudaErrors(cudaGetLastError());
    });

    writeCudaPpm(fb, nx, ny, std::cout);

    checkCudaErrors(cudaFree(states));
    checkCudaErrors(cudaFree(fb));
    scene.free();
    return 0;
}
