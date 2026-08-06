#pragma once

#include "../../src/constants.h"
#include "../../src/hitable.h"
#include "../../src/my_random.h"
#include "../../src/ray.h"

namespace {
constexpr int kMinBouncesBeforeRoulette = 5;
constexpr float kSurvivalMin = 0.05f;
constexpr float kSurvivalMax = 0.95f;
constexpr float kHitEps = 0.001f;

__device__ inline float maxComponent(const vec3& v) {
    return fmaxf(v.r(), fmaxf(v.g(), v.b()));
}

__device__ inline bool applyRussianRoulette(vec3& throughput, int bounce) {
    if (bounce < kMinBouncesBeforeRoulette) {
        return true;
    }
    const float survival = fmaxf(kSurvivalMin, fminf(maxComponent(throughput), kSurvivalMax));
    if (randomFloat(0.0f, 1.0f) > survival) {
        return false;
    }
    throughput /= survival;
    return true;
}
}  // namespace

/**
 * \brief Device path tracer using polymorphic Hitable / Material virtuals.
 *
 * Materials use \p my_random.h device RNG (no flat tables / cuRANDDx).
 */
__device__ inline vec3 pathTrace(const Ray& r, Hitable* world, int maxDepth,
                                 BackgroundFn background = colorBlueWhiteGradient) {
    Ray current = r;
    vec3 radiance(0.0f, 0.0f, 0.0f);
    vec3 throughput(1.0f, 1.0f, 1.0f);

    for (int bounce = 0; bounce < maxDepth; ++bounce) {
        HitRecord hr;
        if (!world->hit(current, kHitEps, infinity, hr)) {
            radiance += throughput * background(current);
            return radiance;
        }

        if (hr.matPtr == nullptr) {
            return radiance;
        }

        radiance += throughput * hr.matPtr->emit(hr.u, hr.v, hr.p);

        Ray scattered;
        vec3 attenuation;
        if (!hr.matPtr->scatter(current, hr, attenuation, scattered)) {
            return radiance;
        }

        throughput = throughput * attenuation;
        if (!applyRussianRoulette(throughput, bounce + 1)) {
            return radiance;
        }
        current = scattered;
    }

    return radiance;
}
