#pragma once

#include"cuda_runtime.h"

__global__ void render(float* fb, int maxX, int maxY){
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)){return;}

    int pixelIdx = j*maxX*3 + i*3;
    fb[pixelIdx] = float(i) / maxX;
    fb[pixelIdx + 1] = float(j) / maxY;
    fb[pixelIdx + 2] = 0.2;
}