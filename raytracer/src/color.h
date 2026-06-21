#pragma once

#include "hitable_list.h"
#include <algorithm>
#include <float.h>
#include "my_random.h"

namespace {
constexpr int kMinBouncesBeforeRoulette = 5;
constexpr double kSurvivalMin = 0.05; ///< avoids throughput / survival blowing up when survival -> 0
constexpr double kSurvivalMax = 0.95; ///< makes sure atleast 5% of rays die (useful for optimizing bright scenes)
constexpr double kHitEps = 0.001; ///< minimum t parameter of the ray to count as a hit

/** 
 * \brief determines maximum component of a vector
 * \sa applyRussianRoulette; used to determine throughput brightness 
 */
inline double maxComponent(const vec3& v) {
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
    const double survival = std::max(kSurvivalMin, std::min(maxComponent(throughput), kSurvivalMax));
    if (randomDouble(0.0, 1.0) > survival) {
        return false;
    }
    throughput /= survival;
    return true;
}
}  // namespace

/**
 * \brief Debug shading from surface normals.
 * \returns Normal map color on hit; sky gradient on miss.
 */
inline vec3 color(const Ray& r, Hitable* world, int depth, int bounce = 0) {
    (void)depth;
    (void)bounce;
    HitRecord hr;
    if (world->hit(r, kHitEps, MAXFLOAT, hr)) {
        return 0.5 * vec3(hr.normal.x() + 1.0, hr.normal.y() + 1.0, hr.normal.z() + 1.0);
    }
    return colorBlueWhiteGradient(r);
}

/**
 * \brief Path tracing integrator; material behavior comes from Material::scatter at each hit.
 * \returns Estimated radiance along \p r with throughput-based Russian roulette
 * \copydoc applyRussianRoulette
 */
inline vec3 pathTrace(const Ray& r, Hitable* world, int maxDepth) {
    Ray current = r;
    vec3 throughput(1, 1, 1);

    for (int bounce = 0; bounce < maxDepth; ++bounce) {
        HitRecord hr;
        if (!world->hit(current, kHitEps, MAXFLOAT, hr)) {
            return throughput * colorBlueWhiteGradient(current);
        }

        Ray scattered;
        vec3 attenuation;
        if (!hr.matPtr || !hr.matPtr->scatter(current, hr, attenuation, scattered)) {
            return vec3(0, 0, 0);
        }

        throughput = throughput * attenuation;
        if (!applyRussianRoulette(throughput, bounce + 1)) {
            return vec3(0, 0, 0);
        }
        current = scattered;
    }

    return vec3(0, 0, 0);
}
