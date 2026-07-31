#pragma once

#include <cmath>

#include "aabb.h"
#include "hitable.h"
#include "material.h"

namespace {
    constexpr float kPlaneParallelEps = 1e-8;
} //namespace

/**
 * \brief Planar parallelogram spanned by corner \p q and edge vectors \p u, \p v.
 */
class Quad : public Hitable {
public:
    Quad(const vec3& q, const vec3& u, const vec3& v, MaterialPtr mat)
        : q(q), u(u), v(v), mat(std::move(mat)) {
        setBoundingBox();

        vec3 n = cross(u,v);
        normal = unit_vector(n);
        d = dot(normal, q);
        w = n / dot(n,n);

        

    }

    bool hit(const Ray& r, float tMin, float tMax, HitRecord& hr) override {
        const float denom = dot(normal, r.direction());
        // Reject only grazing rays; accept both sides of the plane (RTIOW).
        if (std::fabs(denom) < kPlaneParallelEps) {
            return false;
        }
        const float t = (d - dot(normal, r.origin())) / denom;

        if (t < tMin || t > tMax) {
            return false;
        }
        const vec3 intersection = r.point_at_parameter(t);
        const vec3 planarHit = intersection - q;
        const float alpha = dot(w, cross(planarHit, v));
        const float beta = dot(w, cross(u, planarHit));

        if (!inInterior(alpha, beta, hr)) {
            return false;
        }

        hr.t = t;
        hr.p = intersection;
        hr.setFaceNormal(r, normal);
        hr.matPtr = mat;
        return true;
    }
    AABB boundingBox() const override { return aabb; }
    // centroid() inherits AABB midpoint from Hitable

private:
    /** \brief boundingBox setter; unique behavior where aabb is cached */
    void setBoundingBox() {
        const AABB diag1(q, q + u + v);
        const AABB diag2(q + u, q + v);
        aabb = AABB(diag1, diag2);
    }

    /** \brief Sets \p hr.u/\p hr.v when \p a and \p b lie in [0,1]. */
    bool inInterior(float a, float b, HitRecord& hr) {
        if (a < 0 || a > 1 || b < 0 || b > 1) {
            return false;
        }

        hr.u = a;
        hr.v = b;
        return true;
    }

    vec3 q;
    vec3 u, v;
    vec3 w;
    vec3 normal;
    float d;
    MaterialPtr mat;
    AABB aabb;
};
