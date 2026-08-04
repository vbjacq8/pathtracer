#include "../../cuda/host/check_cuda.cuh"
#include "../../cuda/host/device_scene.cuh"
#include "../../cuda/device/render.cuh"

#include <iostream>

/**
 * Minimal DeviceScene smoke test: one sphere + ground.
 *
 *   ./run_cuda.sh --rebuild shaded_sphere.cu
 */
int main() {
    DeviceScene scene;
    Material* red = scene.addLambertian(vec3(1.0f, 0.0f, 0.0f));
    Material* ground = scene.addLambertian(vec3(0.8f, 0.8f, 0.0f));
    scene.addSphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, red);
    scene.addSphere(vec3(0.0f, -100.5f, -1.0f), 100.0f, ground);
    Hitable* world = scene.buildWorld();

    const int nx = 200;
    const int ny = 100;
    vec3 lowerLeftCorner(-2.0f, -1.0f, -1.0f);
    vec3 horizontal(4.0f, 0.0f, 0.0f);
    vec3 vertical(0.0f, 2.0f, 0.0f);
    vec3 origin(0.0f, 0.0f, 0.0f);

    vec3* fb = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&fb), size_t(nx * ny) * sizeof(vec3)));

    dim3 blocks(nx / 8 + 1, ny / 8 + 1);
    dim3 threads(8, 8);
    render<<<blocks, threads>>>(fb, nx, ny, lowerLeftCorner, horizontal, vertical, origin, nullptr);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny - 1; j >= 0; --j) {
        for (int i = 0; i < nx; ++i) {
            const vec3& p = fb[j * nx + i];
            std::cout << int(p[0] * 255.99f) << " " << int(p[1] * 255.99f) << " "
                      << int(p[2] * 255.99f) << "\n";
        }
    }

    checkCudaErrors(cudaFree(fb));
    scene.free();
    return 0;
}
