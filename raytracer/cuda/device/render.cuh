#pragma once

#include <cuda_runtime.h>

#include "../../src/vec3.h"

__global__ void render(vec3* fb, int maxX, int maxY) {
    int i = threadIdx.x + blockIdx.x * blockDim.x;
    int j = threadIdx.y + blockIdx.y * blockDim.y;
    if ((i >= maxX) || (j >= maxY)) {
        return;
    }

    int pixelIdx = j * maxX + i;
    fb[pixelIdx] = vec3(float(i) / maxX, float(j) / maxY, 0.2);
}
