#pragma once

#include "constants.h"
#include "hitable_list.h"
#include "material.h"
#include "my_random.h"

#include <algorithm>

namespace {
constexpr int kMinBouncesBeforeRoulette = 5;
constexpr float kSurvivalMin = 0.05; ///< avoids throughput / survival blowing up when survival -> 0
constexpr float kSurvivalMax = 0.95; ///< makes sure atleast 5% of rays die (useful for optimizing bright scenes)
constexpr float kHitEps = 0.001; ///< minimum t parameter of the ray to count as a hit

/** 
 * \brief determines maximum component of a vector
 * \sa applyRussianRoulette; used to determine throughput brightness 
 */
inline float maxComponent(const vec3& v) {
    return std::max(v.r(), std::max(v.g(), v.b()));
}

/**
 * \brief Throughput-based Russian roulette after \p bounce. 
 * \param throughput path weight updated with 1/survival when the path continues; side-effect based functionality
 * \returns false when the path should terminate
 */
inline bool applyRussianRoulette(vec3& throughput, int bounce) {
    if (bounce < kMinBouncesBeforeRoulette) {
        return true;
    }
    const float survival = std::max(kSurvivalMin, std::min(maxComponent(throughput), kSurvivalMax));
    if (randomFloat(0.0, 1.0) > survival) {
        return false;
    }
    throughput /= survival;
    return true;
}
}  // namespace

/**
 * \brief Debug shading from surface normals.
 * \returns Normal map color on hit; \p background on miss.
 */
inline vec3 color(const Ray& r, Hitable* world, int depth,
                  BackgroundFn background = colorBlueWhiteGradient) {
    (void)depth;
    HitRecord hr;
    if (world->hit(r, kHitEps, infinity, hr)) {
        return 0.5 * vec3(hr.normal.x() + 1.0, hr.normal.y() + 1.0, hr.normal.z() + 1.0);
    }
    return background(r);
}

/**
 * \brief Path tracing integrator; material behavior comes from Material::scatter at each hit.
 * \param background miss-ray radiance strategy (\p colorBlueWhiteGradient, \p colorVoid, …)
 * \returns Estimated radiance along \p r with throughput-based Russian roulette
 * \copydoc applyRussianRoulette
 */
inline vec3 pathTrace(const Ray& r, Hitable* world, int maxDepth,
                      BackgroundFn background = colorBlueWhiteGradient) {
    Ray current = r;
    vec3 radiance(0, 0, 0);   // accumulated outgoing light toward the camera
    vec3 throughput(1, 1, 1); // product of attenuations along the path so far

    for (int bounce = 0; bounce < maxDepth; ++bounce) {
        HitRecord hr;
        if (!world->hit(current, kHitEps, infinity, hr)) {
            // Miss: sky/void contributes, scaled by how much of the path weight remains.
            radiance += throughput * background(current);
            return radiance;
        }

        if (!hr.matPtr) {
            return radiance;
        }

        // Emitters add light here (DiffuseLight / Light); ordinary materials add 0.
        radiance += throughput * hr.matPtr->emit(hr.u, hr.v, hr.p);

        Ray scattered;
        vec3 attenuation;
        if (!hr.matPtr->scatter(current, hr, attenuation, scattered)) {
            // Absorbed / pure emitter: no further bounces.
            return radiance;
        }

        // Throughput stays multiplicative only — never add emission into it.
        throughput = throughput * attenuation;
        if (!applyRussianRoulette(throughput, bounce + 1)) {
            return radiance;
        }
        current = scattered;
    }

    return radiance;
}
