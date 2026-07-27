#ifndef BOX_AABB_IMPL_H
#define BOX_AABB_IMPL_H

#include "hitable_list.h"
#include "material.h"
#include "quad.h"

/**
 * \brief Box primitive using AABB to keep track of corners; faster hit reads (i think), use when box will always be aligned with axes
 */
class BoxAABBImpl: public Hitable {
public:
    BoxAABBImpl(const AABB& b, MaterialPtr mPtr) : bounds(b), matPtr(std::move(mPtr)) {}

    BoxAABBImpl(const vec3& min, const vec3& max, MaterialPtr mPtr) : bounds(AABB(min, max)), matPtr(mPtr) {}

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override;
    AABB boundingBox() const override;

    AABB bounds;
    MaterialPtr matPtr;
};

/**
 * \brief Ray-box intersection via AABB slab test.
 * \copydoc Hitable::hit
 */
inline bool BoxAABBImpl::hit(const Ray& r, double tMin, double tMax, HitRecord& hr) {
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

inline AABB BoxAABBImpl::boundingBox() const {
    return bounds;
}

#endif
