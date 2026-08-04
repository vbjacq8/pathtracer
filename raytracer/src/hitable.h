#ifndef HITABLE_H
#define HITABLE_H

#include "aabb.h"
#include "cuda_annot.h"
#include "ray.h"

#include <memory>

class Material;

/**
 * \brief Intersection data written by Hitable::hit.
 */
struct HitRecord {
    float t;
    vec3 p;
    vec3 normal;
    Material* matPtr = nullptr;
    float u;
    float v;
    bool frontFace;

    /** Sets \p normal to point against \p r; records whether the geometric normal faced the ray. */
    PATHTRACER_HD void setFaceNormal(const Ray& r, const vec3& outwardNormal) {
        frontFace = dot(r.direction(), outwardNormal) < 0;
        normal = frontFace ? outwardNormal : (-1.0f * outwardNormal);
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
    PATHTRACER_HD virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& hr) = 0;

    /** \returns Axis-aligned bounds of this primitive. */
    PATHTRACER_HD virtual AABB boundingBox() const = 0;

    /**
     * \returns Centroid used for spatial partitioning.
     * Default: midpoint of \p boundingBox(); override for motion or multi-primitive shapes.
     */
    PATHTRACER_HD virtual vec3 centroid() const {
        const AABB box = boundingBox();
        return 0.5f * (box.min + box.max);
    }
};

using HitablePtr = std::shared_ptr<Hitable>;

#endif
