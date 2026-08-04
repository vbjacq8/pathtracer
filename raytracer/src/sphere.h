#ifndef SPHERE_H
#define SPHERE_H

#include "constants.h"
#include "cuda_annot.h"
#include "hitable.h"
#include "material.h"

/**
 * \brief Sphere primitive with an attached material.
 *
 * Stores a non-owning \p mat pointer (CUDA-friendly). The caller owns the material.
 */
class Sphere : public Hitable {
public:
    PATHTRACER_HD Sphere() : mat(nullptr) {}

    PATHTRACER_HD Sphere(const vec3& staticCenter, float R, Material* material)
        : center(staticCenter, vec3(0, 0, 0)), radius(R), mat(material) {}

    PATHTRACER_HD Sphere(const vec3& center1, const vec3& center2, float R, Material* material)
        : center(center1, center2 - center1), radius(R), mat(material) {}

    PATHTRACER_HD bool hit(const Ray& r, float tMin, float tMax, HitRecord& hr) override;
    PATHTRACER_HD AABB boundingBox() const override;
    PATHTRACER_HD vec3 centroid() const override;

private:
    Ray center;
    float radius;
    Material* mat;

    /**
     * \brief Maps a unit-sphere direction to texture (u, v) in [0, 1].
     * \param p unit vector from sphere center to the hit (same as outward normal)
     * \param u azimuth in [0, 1], starting at -x
     * \param v zenith in [0, 1], starting at -y
     */
    PATHTRACER_HD static void getSphereUV(const vec3& p, float& u, float& v) {
        // p must be a unit vector; acos domain is [-1, 1].
        const float theta = acosf(-p.y());
        const float phi = atan2f(-p.z(), p.x()) + pi;
        u = phi / (2 * pi);
        v = theta / pi;
    }

    PATHTRACER_HD void fillHitRecord(const Ray& r, float t, const vec3& currentCenter,
                                     HitRecord& hr) const {
        hr.t = t;
        hr.p = r.point_at_parameter(t);
        const vec3 outwardNormal = (hr.p - currentCenter) / radius;
        hr.setFaceNormal(r, outwardNormal);
        hr.matPtr = mat;
        getSphereUV(outwardNormal, hr.u, hr.v);
    }
};

/**
 * \brief Ray-sphere intersection for Sphere.
 * \copydoc Hitable::hit
 */
PATHTRACER_HD inline bool Sphere::hit(const Ray& r, float tMin, float tMax, HitRecord& hr) {
    // Use the *incoming* ray's shutter time, not center.time() (which is always 0).
    const vec3 currentCenter = center.point_at_parameter(r.time());
    const vec3 oc = r.origin() - currentCenter;
    const float b = 2.0f * dot(oc, r.direction());
    const float a = dot(r.direction(), r.direction());
    const float c = dot(oc, oc) - radius * radius;
    const float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) {
        return false;
    }

    float temp = (-b - sqrtf(discriminant)) / (2.0f * a);
    if (temp > tMin && temp < tMax) {
        fillHitRecord(r, temp, currentCenter, hr);
        return true;
    }
    temp = (-b + sqrtf(discriminant)) / (2.0f * a);
    if (temp > tMin && temp < tMax) {
        fillHitRecord(r, temp, currentCenter, hr);
        return true;
    }
    return false;
}

PATHTRACER_HD inline vec3 Sphere::centroid() const {
    // Motion blur: midpoint of the shutter path, not the union-AABB center.
    return 0.5f * (center.point_at_parameter(0) + center.point_at_parameter(1));
}

PATHTRACER_HD inline AABB Sphere::boundingBox() const {
    const vec3 rVec(radius, radius, radius);
    const AABB at0(center.point_at_parameter(0) - rVec, center.point_at_parameter(0) + rVec);
    const AABB at1(center.point_at_parameter(1) - rVec, center.point_at_parameter(1) + rVec);
    return AABB(at0, at1);
}

#endif
