#ifndef BOX_AABB_IMPL_H
#define BOX_AABB_IMPL_H

#include "hitable_list.h"
#include "material.h"
#include "quad.h"

/**
 * \brief Box primitive using AABB to keep track of corners; faster hit reads (i think), use when box will always be aligned with axes
 *
 * Stores a non-owning \p matPtr. The caller owns the material.
 */
class BoxAABBImpl: public Hitable {
public:
    BoxAABBImpl(const AABB& b, Material* mPtr) : bounds(b), matPtr(mPtr) {}

    BoxAABBImpl(const vec3& min, const vec3& max, Material* mPtr) : bounds(AABB(min, max)), matPtr(mPtr) {}

    bool hit(const Ray& r, float tMin, float tMax, HitRecord& hr) override;
    AABB boundingBox() const override;

    AABB bounds;
    Material* matPtr;
};

/**
 * \brief Ray-box intersection via AABB slab test.
 * \copydoc Hitable::hit
 */
inline bool BoxAABBImpl::hit(const Ray& r, float tMin, float tMax, HitRecord& hr) {
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
