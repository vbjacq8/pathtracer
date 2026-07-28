#include"../../cuda/host/check_cuda.cuh"
#include"../../cuda/device/render.cuh"

/**
 * \brief first image generated using CUDA. Use nvcc color_gradient.cu -o color_gradient and pipe to exec/cuda
 

*/

int main(){
    int nx = 200;
    int ny = 100;

    int numPixels = nx * ny;
    int fbSize = numPixels * 3 * sizeof(float);

    float* fb;
    checkCudaErrors(cudaMallocManaged((void**) &fb, fbSize));
    int tx = 8;
    int ty = 8;

    dim3 blocks(nx/tx +1, ny/ty + 1);
    dim3 threads(tx, ty);

    render<<<blocks, threads>>>(fb, nx, ny);

    std::cout << "P3\n" << nx << " " << ny << "\n255\n";
    for (int j = ny-1; j >=0; --j){
        for (int i = 0; i < nx; ++i){
            size_t pixelIdx = (j * nx + i) * 3;
            float r = fb[pixelIdx];
            float g = fb[pixelIdx + 1];
            float b = fb[pixelIdx + 2];
            int ir = int(r * 255.99);
            int ig = int(g * 255.99);
            int ib = int(b * 255.99);
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    checkCudaErrors(cudaFree(fb));

}