#pragma once

#include "ray.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr double kAxisParallelEps = 1e-8;
constexpr double kSlabHitEps = 1e-8;
}  // namespace

/**
 * \brief Axis-aligned bounding box defined by two corners.
 *
 * Corners may be passed in any order; constructors normalize so min <= max per axis.
 */
struct AABB {
    vec3 min;
    vec3 max;

    AABB() = default;

    /** \brief Builds a box from two opposite corners (order does not matter). */
    AABB(const vec3& a, const vec3& b)
        : min(std::fmin(a[0], b[0]), std::fmin(a[1], b[1]), std::fmin(a[2], b[2])),
          max(std::fmax(a[0], b[0]), std::fmax(a[1], b[1]), std::fmax(a[2], b[2])) {}

    /** \brief Union of two boxes. */
    AABB(const AABB& a, const AABB& b)
        : min(std::fmin(a.min[0], b.min[0]), std::fmin(a.min[1], b.min[1]), std::fmin(a.min[2], b.min[2])),
          max(std::fmax(a.max[0], b.max[0]), std::fmax(a.max[1], b.max[1]), std::fmax(a.max[2], b.max[2])) {}

    /**
     * \brief Slab test for whether \p r intersects this box within (\p tMin, \p tMax).
     */
    bool intersects(const Ray& r, double tMin, double tMax) const {
        double tHit = 0;
        vec3 normal;
        return slabInterval(r, tMin, tMax, tHit, normal);
    }

    /**
     * \brief Slab test that records the nearest hit distance and face normal opposing the ray.
     */
    bool slabInterval(const Ray& r, double tMin, double tMax, double& tHit, vec3& normal) const {
        double t0s[3];
        double t1s[3];

        for (int axis = 0; axis < 3; ++axis) {
            const double origin = r.origin()[axis];
            const double direction = r.direction()[axis];

            if (std::fabs(direction) < kAxisParallelEps) {
                if (origin < min[axis] || origin > max[axis]) {
                    return false;
                }
                t0s[axis] = -1e30;
                t1s[axis] = 1e30;
                continue;
            }

            double t0 = (min[axis] - origin) / direction;
            double t1 = (max[axis] - origin) / direction;
            if (t0 > t1) {
                std::swap(t0, t1);
            }
            t0s[axis] = t0;
            t1s[axis] = t1;
            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMax <= tMin) {
                return false;
            }
        }

        if (tMax < 0) {
            return false;
        }

        const bool hitFromOutside = tMin > 0;
        tHit = (hitFromOutside) ? tMin : tMax;

        normal = vec3(0, 0, 0);
        for (int axis = 0; axis < 3; ++axis) {
            const double faceT = hitFromOutside ? t0s[axis] : t1s[axis];
            if (std::fabs(tHit - faceT) < kSlabHitEps) {
                normal[axis] = (r.direction()[axis] > 0) ? -1.0 : 1.0;
                return true;
            }
        }
        return false;
    }
};
