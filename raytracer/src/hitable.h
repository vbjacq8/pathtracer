#ifndef HITABLE_H
#define HITABLE_H

#include "ray.h"
#include "aabb.h"

#include <memory>

class Material;

/**
 * \brief Intersection data written by Hitable::hit.
 */
struct HitRecord {
    double t;
    vec3 p;
    vec3 normal;
    std::shared_ptr<Material> matPtr;
    double u;
    double v;
    bool frontFace;

    /** Sets \p normal to point against \p r; records whether the geometric normal faced the ray. */
    void setFaceNormal(const Ray& r, const vec3& outwardNormal) {
        frontFace = dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : (-1.0 * outwardNormal);
    }
    
};

/**
 * \brief Abstract surface that a ray may intersect.
 */
class Hitable {
public:
    virtual ~Hitable() = default;

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

    /**
     * \returns Centroid used for spatial partitioning.
     * Default: midpoint of \p boundingBox(); override for motion or multi-primitive shapes.
     */
    virtual vec3 centroid() const {
        const AABB box = boundingBox();
        return 0.5 * (box.min + box.max);
    }
};

using HitablePtr = std::shared_ptr<Hitable>;

#endif
