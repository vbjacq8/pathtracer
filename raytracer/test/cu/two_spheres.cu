#include "../../cuda/host/check_cuda.cuh"
#include "../../cuda/host/device_scene.cuh"
#include "../../cuda/device/render.cuh"

#include <iostream>

/**
 * Host-composed CUDA scene (no per-file createWorld kernel).
 *
 *   source raytracer/cuda/env/longleaf_modules.sh
 *   ./run_cuda.sh --rebuild two_spheres.cu
 */
int main() {
    DeviceScene scene;

    Material* blue = scene.addLambertian(vec3(0.1f, 0.2f, 0.5f));
    Material* ground = scene.addLambertian(vec3(0.8f, 0.8f, 0.0f));
    Material* metal = scene.addMetal(vec3(0.8f, 0.8f, 0.8f), 0.0f);
    Material* glass = scene.addDielectric(1.5f);

    scene.addSphere(vec3(0.0f, 0.0f, -1.0f), 0.5f, blue);
    scene.addSphere(vec3(0.0f, -100.5f, -1.0f), 100.0f, ground);
    scene.addSphere(vec3(1.0f, 0.0f, -1.0f), 0.5f, metal);
    scene.addSphere(vec3(-1.0f, 0.0f, -1.0f), 0.5f, glass);

    Hitable* world = scene.buildWorld();
    (void)world;
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
