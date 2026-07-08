#ifndef HITABLE_H
#define HITABLE_H

#include "ray.h"
#include "aabb.h"

class Material;

/**
 * \brief Intersection data written by Hitable::hit.
 */
struct HitRecord {
    double t;
    vec3 p;
    vec3 normal;
    Material* matPtr;
};

/**
 * \brief Abstract surface that a ray may intersect.
 */
class Hitable {
public:
    /**
     * \brief Tests whether \p r hits this surface within (\p tMin, \p tMax).
     * \param r ray to test
     * \param tMin minimum valid hit distance
     * \param tMax maximum valid hit distance
     * \param hr hit record updated ON SUCCESS; if no hit, hitRecord not updated.
     * \returns true when a hit is found
     */
    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) = 0;

    /** \returns Axis-aligned bounds of this primitive. */
    virtual AABB boundingBox() const = 0;
    /** \returns Centroid used for spatial partitioning. */
    virtual vec3 centroid() const = 0;
};

#endif
