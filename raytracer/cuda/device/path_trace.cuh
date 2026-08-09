#pragma once

#include "../../src/constants.h"
#include "../../src/ray.h"

#include "hitable_rec.cuh"
#include "material_rec.cuh"
#include "my_random.cuh"
#include "scatter.cuh"

namespace {
constexpr int kMinBouncesBeforeRoulette = 5;
constexpr float kSurvivalMin = 0.05f;
constexpr float kSurvivalMax = 0.95f;
constexpr float kHitEps = 0.001f;

__device__ inline float maxComponent(const vec3& v) {
    return fmaxf(v.r(), fmaxf(v.g(), v.b()));
}

__device__ inline bool applyRussianRoulette(vec3& throughput, int bounce, RNG* states, int tid) {
    if (bounce < kMinBouncesBeforeRoulette) {
        return true;
    }
    const float survival = fmaxf(kSurvivalMin, fminf(maxComponent(throughput), kSurvivalMax));
    if (pathtracer_cuda_rng::uniformFloat(0.0f, 1.0f, states, tid) > survival) {
        return false;
    }
    throughput /= survival;
    return true;
}
}  // namespace

/**
 * \brief Device path tracer using flat HitableRec / MaterialRec / TextureRec tables.
 */
__device__ inline vec3 pathTrace(const Ray& r, const HitableRec* hitables, int hitableCount,
                                 const MaterialRec* materials, const TextureRec* textures,
                                 int maxDepth, RNG* states, int tid,
                                 BackgroundFn background = colorBlueWhiteGradient) {
    Ray current = r;
    vec3 radiance(0.0f, 0.0f, 0.0f);
    vec3 throughput(1.0f, 1.0f, 1.0f);

    for (int bounce = 0; bounce < maxDepth; ++bounce) {
        HitRecord hr;
        if (!hitScene(hitables, hitableCount, current, kHitEps, infinity, hr)) {
            radiance += throughput * background(current);
            return radiance;
        }

        if (hr.matIndex < 0 || materials == nullptr) {
            return radiance;
        }

        const MaterialRec& mat = materials[hr.matIndex];
        radiance += throughput * emitMaterial(mat, textures, hr.u, hr.v, hr.p);

        Ray scattered;
        vec3 attenuation;
        if (!scatterMaterial(mat, textures, current, hr, attenuation, scattered, states, tid)) {
            return radiance;
        }

        throughput = throughput * attenuation;
        if (!applyRussianRoulette(throughput, bounce + 1, states, tid)) {
            return radiance;
        }
        current = scattered;
    }

    return radiance;
}
