#pragma once

#include "types.cuh"

/**
 * Stub for the device integrator. Implement after scene buffers exist.
 * Signature is a placeholder — change freely.
 */
__device__ inline Vec3 pathTraceStub(const Ray& /*r*/, int /*maxDepth*/) {
    return Vec3(0.0f, 0.0f, 0.0f);
}
