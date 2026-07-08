#pragma once

#include "ray.h"
#include <algorithm>

/**
 * \brief Axis-aligned bounding box to defined by two "corners"; Simplifies collision detection
 * \sa Hitable
 */
struct AABB {
    vec3 min;
    vec3 max;

    /**
     * \brief Slab test for whether \p r intersects this box within (\p tMin, \p tMax).
     */
    bool intersects(const Ray& r, double tMin, double tMax) const {
        for (int axis = 0; axis < 3; axis++) {
            const double origin = r.origin()[axis];
            const double direction = r.direction()[axis];
            double t0 = (min[axis] - origin) / direction;
            double t1 = (max[axis] - origin) / direction;
            if (direction < 0.0) {
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
};
