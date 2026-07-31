#pragma once

#include "constants.h"
#include "ray.h"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kAxisParallelEps = 1e-8;
constexpr float kSlabHitEps = 1e-8;
constexpr float kPadEps = 1e-4;
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
          max(std::fmax(a[0], b[0]), std::fmax(a[1], b[1]), std::fmax(a[2], b[2])) {
            padToMinimums();
          }

    /** \brief Union of two boxes. */
    AABB(const AABB& a, const AABB& b)
        : min(std::fmin(a.min[0], b.min[0]), std::fmin(a.min[1], b.min[1]), std::fmin(a.min[2], b.min[2])),
          max(std::fmax(a.max[0], b.max[0]), std::fmax(a.max[1], b.max[1]), std::fmax(a.max[2], b.max[2])) 
          {
            padToMinimums();
          }

    void padToMinimums(){
        vec3 range = max - min;
        if (range.x() < kPadEps){max[0] = min.x() + kPadEps;}
        if (range.y() < kPadEps){max[1] = min.y() + kPadEps;}
        if (range.z() < kPadEps){max[2] = min.z() + kPadEps;}
    }

    /**
     * \brief True if the ray overlaps this box in (\p tMin, \p tMax).
     * Used for BVH culling — must succeed when the ray origin is inside the box.
     */
    bool intersects(const Ray& r, float tMin, float tMax) const {
        for (int axis = 0; axis < 3; ++axis) {
            const float origin = r.origin()[axis];
            const float direction = r.direction()[axis];

            if (std::fabs(direction) < kAxisParallelEps) {
                if (origin < min[axis] || origin > max[axis]) {
                    return false;
                }
                continue;
            }

            float t0 = (min[axis] - origin) / direction;
            float t1 = (max[axis] - origin) / direction;
            if (t0 > t1) {
                std::swap(t0, t1);
            }
            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMax <= tMin) {
                return false;
            }
        }
        return true;
    }

    /**
     * \brief Slab test that records the nearest forward surface hit and opposing face normal.
     * Used by the Box primitive. Geometric enter/exit are tracked separately from the
     * caller's clipped ray window so interior origins use the exit face.
     */
    bool slabInterval(const Ray& r, float rayTMin, float rayTMax, float& tHit, vec3& normal) const {
        float t0s[3];
        float t1s[3];
        float tEnter = -infinity;  // geometric entry (max of per-axis t0)
        float tExit = +infinity;   // geometric exit  (min of per-axis t1)

        for (int axis = 0; axis < 3; ++axis) {
            const float origin = r.origin()[axis];
            const float direction = r.direction()[axis];

            if (std::fabs(direction) < kAxisParallelEps) {
                if (origin < min[axis] || origin > max[axis]) {
                    return false;
                }
                t0s[axis] = -infinity;
                t1s[axis] = +infinity;
                continue;
            }

            float t0 = (min[axis] - origin) / direction;
            float t1 = (max[axis] - origin) / direction;
            if (t0 > t1) {
                std::swap(t0, t1);
            }
            t0s[axis] = t0;
            t1s[axis] = t1;
            tEnter = std::max(tEnter, t0);
            tExit = std::min(tExit, t1);
            if (tExit <= tEnter) {
                return false;
            }
        }

        // Outside: hit entry face. Inside (tEnter <= 0): hit exit face ahead of the origin.
        const bool hitFromOutside = tEnter > 0;
        tHit = hitFromOutside ? tEnter : tExit;

        if (tHit <= rayTMin || tHit >= rayTMax) {
            return false;
        }

        normal = vec3(0, 0, 0);
        for (int axis = 0; axis < 3; ++axis) {
            const float faceT = hitFromOutside ? t0s[axis] : t1s[axis];
            if (std::fabs(tHit - faceT) < kSlabHitEps) {
                normal[axis] = (r.direction()[axis] > 0) ? -1.0 : 1.0;
                return true;
            }
        }
        return false;
    }

    AABB operator+(const vec3& v1){
        return AABB(min + v1, max+v1);
    }

};
