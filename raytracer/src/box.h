#ifndef BOX_H
#define BOX_H

#include "hitable.h"
#include "material.h"

/**
 * \brief Axis-aligned box primitive.
 */
class Box : public Hitable {
public:
    Box(const AABB& b, MaterialPtr mPtr) : bounds(b), matPtr(std::move(mPtr)) {}
    Box(const vec3& min, const vec3& max, MaterialPtr mPtr)
        : bounds(AABB{min, max}), matPtr(std::move(mPtr)) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override;
    AABB boundingBox() const override;

    AABB bounds;
    MaterialPtr matPtr;
};

/**
 * \brief Ray-box intersection via AABB slab test.
 * \copydoc Hitable::hit
 */
inline bool Box::hit(const Ray& r, double tMin, double tMax, HitRecord& hr) {
    vec3 outwardNormal;
    if (!bounds.slabInterval(r, tMin, tMax, hr.t, outwardNormal)) {
        return false;
    }
    hr.p = r.point_at_parameter(hr.t);
    hr.setFaceNormal(r, outwardNormal);
    hr.matPtr = matPtr;
    hr.u = 0;
    hr.v = 0;
    return true;
}

inline AABB Box::boundingBox() const {
    return bounds;
}

#endif
