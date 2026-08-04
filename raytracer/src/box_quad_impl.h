#ifndef BOX_Quad_IMPL_H
#define BOX_Quad_IMPL_H

#include "hitable_list.h"
#include "material.h"
#include "quad.h"

/**
 * \brief Box primitive using 6 \sa Quad primitives as sides. Flexible even when sides are not aligned with axes.
 *
 * Stores a non-owning \p matPtr shared by all sides. The caller owns the material.
 */
class BoxQuadImpl: public Hitable {
public:
    BoxQuadImpl(const AABB& b, Material* mPtr) : matPtr(mPtr) {
        min = b.min;
        max = b.max;
        vec3 dr = max - min;
        vec3 dx = vec3(dr[0], 0, 0);
        vec3 dy = vec3(0, dr[1], 0);
        vec3 dz = vec3(0, 0, dr[2]);

        sides->add(std::make_shared<Quad>(vec3(min.x(), min.y(), max.z()),  dx,  dy, matPtr)); // front
        sides->add(std::make_shared<Quad>(vec3(max.x(), min.y(), max.z()), -dz,  dy, matPtr)); // right
        sides->add(std::make_shared<Quad>(vec3(max.x(), min.y(), min.z()), -dx,  dy, matPtr)); // back
        sides->add(std::make_shared<Quad>(vec3(min.x(), min.y(), min.z()),  dz,  dy, matPtr)); // left
        sides->add(std::make_shared<Quad>(vec3(min.x(), max.y(), max.z()),  dx, -dz, matPtr)); // top
        sides->add(std::make_shared<Quad>(vec3(min.x(), min.y(), min.z()),  dx,  dz, matPtr)); // bottom
    }

    // Must use a delegating ctor initializer — a body call creates a temporary and throws it away.
    BoxQuadImpl(const vec3& min, const vec3& max, Material* mPtr)
        : BoxQuadImpl(
            AABB(
                vec3(std::fmin(min.x(),max.x()), std::fmin(min.y(),max.y()), std::fmin(min.z(),max.z())),
                vec3(std::fmax(min.x(),max.x()), std::fmax(min.y(),max.y()), std::fmax(min.z(),max.z()))
             ),
             mPtr) {}

    bool hit(const Ray& r, float tMin, float tMax, HitRecord& hr) override;
    AABB boundingBox() const override;


    Material* matPtr;
    vec3 min, max;
    //Is this allowed lol
    std::shared_ptr<HitableList> sides = std::make_shared<HitableList>();
};

/**
 * \brief Ray-box intersection via AABB slab test.
 * \copydoc Hitable::hit
 */
inline bool BoxQuadImpl::hit(const Ray& r, float tMin, float tMax, HitRecord& hr) {
    return sides->hit(r, tMin, tMax, hr);
}

inline AABB BoxQuadImpl::boundingBox() const {
    return AABB(min, max);
}

#endif
