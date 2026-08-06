#pragma once

#include "../../src/constants.h"
#include "../../src/hitable.h"
#include "../../src/ray.h"
#include "../../src/vec3.h"

/**
 * Flat device hitable records — no virtuals; dispatch via HitableType switch.
 */

enum class HitableType : int {
    Sphere = 0,       ///< static sphere (center0 only)
    MovingSphere = 1, ///< linear motion center0 → center1 over shutter [0,1]
    Quad = 2          ///< parallelogram at q with edges u, v
};

/**
 * \brief One primitive in the flat hitable table.
 *
 * Field layout by type:
 * - Sphere:        center0, radius, matIndex
 * - MovingSphere:  center0, center1, radius, matIndex
 * - Quad:          q=center0, u=center1, v=edgeV; normal/w/d precomputed
 */
struct HitableRec {
    HitableType type = HitableType::Sphere;
    int matIndex = -1;
    float radius = 0.0f;
    float d = 0.0f; ///< quad plane offset
    vec3 center0{0.0f, 0.0f, 0.0f};
    vec3 center1{0.0f, 0.0f, 0.0f}; ///< moving end / quad edge u
    vec3 edgeV{0.0f, 0.0f, 0.0f};   ///< quad edge v
    vec3 normal{0.0f, 0.0f, 0.0f};  ///< quad unit normal
    vec3 w{0.0f, 0.0f, 0.0f};       ///< quad barycentric helper
};

inline HitableRec makeSphereRec(const vec3& center, float radius, int matIndex) {
    HitableRec h;
    h.type = HitableType::Sphere;
    h.center0 = center;
    h.radius = radius;
    h.matIndex = matIndex;
    return h;
}

inline HitableRec makeMovingSphereRec(const vec3& c0, const vec3& c1, float radius, int matIndex) {
    HitableRec h;
    h.type = HitableType::MovingSphere;
    h.center0 = c0;
    h.center1 = c1;
    h.radius = radius;
    h.matIndex = matIndex;
    return h;
}

inline HitableRec makeQuadRec(const vec3& q, const vec3& u, const vec3& v, int matIndex) {
    HitableRec h;
    h.type = HitableType::Quad;
    h.center0 = q;
    h.center1 = u;
    h.edgeV = v;
    h.matIndex = matIndex;
    const vec3 n = cross(u, v);
    h.normal = unit_vector(n);
    h.d = dot(h.normal, q);
    h.w = n / dot(n, n);
    return h;
}

namespace {
constexpr float kPlaneParallelEps = 1e-8f;

__device__ inline void getSphereUV(const vec3& p, float& u, float& v) {
    const float theta = acosf(-p.y());
    const float phi = atan2f(-p.z(), p.x()) + pi;
    u = phi / (2.0f * pi);
    v = theta / pi;
}

__device__ inline void fillSphereHit(const Ray& r, float t, const vec3& currentCenter, float radius,
                                     int matIndex, HitRecord& hr) {
    hr.t = t;
    hr.p = r.point_at_parameter(t);
    const vec3 outwardNormal = (hr.p - currentCenter) / radius;
    hr.setFaceNormal(r, outwardNormal);
    hr.matPtr = nullptr;
    hr.matIndex = matIndex;
    getSphereUV(outwardNormal, hr.u, hr.v);
}

__device__ inline bool hitSphereRec(const HitableRec& h, const Ray& r, float tMin, float tMax,
                                    HitRecord& hr) {
    vec3 currentCenter = h.center0;
    if (h.type == HitableType::MovingSphere) {
        // Shutter time in [0,1]: lerp center0 → center1 (matches Sphere motion Ray).
        currentCenter = h.center0 + r.time() * (h.center1 - h.center0);
    }
    const vec3 oc = r.origin() - currentCenter;
    const float b = 2.0f * dot(oc, r.direction());
    const float a = dot(r.direction(), r.direction());
    const float c = dot(oc, oc) - h.radius * h.radius;
    const float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f) {
        return false;
    }
    float temp = (-b - sqrtf(discriminant)) / (2.0f * a);
    if (temp > tMin && temp < tMax) {
        fillSphereHit(r, temp, currentCenter, h.radius, h.matIndex, hr);
        return true;
    }
    temp = (-b + sqrtf(discriminant)) / (2.0f * a);
    if (temp > tMin && temp < tMax) {
        fillSphereHit(r, temp, currentCenter, h.radius, h.matIndex, hr);
        return true;
    }
    return false;
}

__device__ inline bool hitQuadRec(const HitableRec& h, const Ray& r, float tMin, float tMax,
                                  HitRecord& hr) {
    const float denom = dot(h.normal, r.direction());
    if (fabsf(denom) < kPlaneParallelEps) {
        return false;
    }
    const float t = (h.d - dot(h.normal, r.origin())) / denom;
    if (t < tMin || t > tMax) {
        return false;
    }
    const vec3 intersection = r.point_at_parameter(t);
    const vec3 planarHit = intersection - h.center0;
    const float alpha = dot(h.w, cross(planarHit, h.edgeV));
    const float beta = dot(h.w, cross(h.center1, planarHit));
    if (alpha < 0.0f || alpha > 1.0f || beta < 0.0f || beta > 1.0f) {
        return false;
    }
    hr.t = t;
    hr.p = intersection;
    hr.u = alpha;
    hr.v = beta;
    hr.setFaceNormal(r, h.normal);
    hr.matPtr = nullptr;
    hr.matIndex = h.matIndex;
    return true;
}

__device__ inline bool hitOne(const HitableRec& h, const Ray& r, float tMin, float tMax,
                              HitRecord& hr) {
    switch (h.type) {
    case HitableType::Sphere:
    case HitableType::MovingSphere:
        return hitSphereRec(h, r, tMin, tMax, hr);
    case HitableType::Quad:
        return hitQuadRec(h, r, tMin, tMax, hr);
    }
    return false;
}
}  // namespace

/**
 * \brief Closest-hit among a flat hitable table (replaces virtual HitableList::hit).
 */
__device__ inline bool hitScene(const HitableRec* hitables, int count, const Ray& r, float tMin,
                                float tMax, HitRecord& hr) {
    HitRecord temp;
    bool hitAnything = false;
    float closest = tMax;
    if (hitables == nullptr || count <= 0) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (hitOne(hitables[i], r, tMin, closest, temp)) {
            hitAnything = true;
            closest = temp.t;
            hr = temp;
        }
    }
    return hitAnything;
}
