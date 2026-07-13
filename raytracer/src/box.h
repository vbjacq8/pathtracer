#ifndef BOX_H
#define BOX_H

#include "hitable.h"
#include "material.h"

/**
 * \brief Axis-aligned box primitive.
 */
class Box : public Hitable {
public:
    Box(const AABB& b, Material* mPtr) : bounds(b), matPtr(mPtr) {}
    Box(const vec3& min, const vec3& max, Material* mPtr) : bounds(AABB{min, max}), matPtr(mPtr) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override;
    AABB boundingBox() const override;
    vec3 centroid() const override;

    AABB bounds;
    Material* matPtr;
};

/**
 * \brief Ray-box intersection via AABB slab test.
 * \copydoc Hitable::hit
 */
inline bool Box::hit(const Ray& r, double tMin, double tMax, HitRecord& hr) {
    if (!bounds.slabInterval(r, tMin, tMax, hr.t, hr.normal)) {
        return false;
    }
    hr.p = r.point_at_parameter(hr.t);
    hr.matPtr = matPtr;
    return true;
}

inline vec3 Box::centroid() const {
    return (bounds.min + bounds.max) * 0.5;
}

inline AABB Box::boundingBox() const {
    return bounds;
}

#endif
