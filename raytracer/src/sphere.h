#ifndef SPHERE_H
#define SPHERE_H

#include "constants.h"
#include "hitable.h"
#include "material.h"

/**
 * \brief Sphere primitive with an attached material.
 */
class Sphere : public Hitable {
public:
    Sphere() {}

    Sphere(const vec3& staticCenter, float R, MaterialPtr mat)
        : center(staticCenter, vec3(0, 0, 0)), radius(R), matPtr(std::move(mat)) {}

    Sphere(const vec3& center1, const vec3& center2, float R, MaterialPtr mat)
        : center(center1, center2 - center1), radius(R), matPtr(std::move(mat)) {}

    virtual bool hit(const Ray& r, float tMin, float tMax, HitRecord& hr) override;
    AABB boundingBox() const override;
    vec3 centroid() const override;
    

private:
    Ray center;
    float radius;
    MaterialPtr matPtr;

    /**
     * \brief Maps a unit-sphere direction to texture (u, v) in [0, 1].
     * \param p unit vector from sphere center to the hit (same as outward normal)
     * \param u azimuth in [0, 1], starting at -x
     * \param v zenith in [0, 1], starting at -y
     */
    static void getSphereUV(const vec3& p, float& u, float& v) {
        // p must be a unit vector; acos domain is [-1, 1].
        const float theta = std::acos(-p.y());
        const float phi = std::atan2(-p.z(), p.x()) + pi;
        u = phi / (2 * pi);
        v = theta / pi;
    }
    
};

/**
 * \brief Ray-sphere intersection for Sphere.
 * \copydoc Hitable::hit
 */
inline bool Sphere::hit(const Ray& r, float tMin, float tMax, HitRecord& hr) {
    // Use the *incoming* ray's shutter time, not center.time() (which is always 0).
    const vec3 currentCenter = center.point_at_parameter(r.time());
    const vec3 oc = r.origin() - currentCenter;
    const float b = 2.0 * dot(oc, r.direction());
    const float a = dot(r.direction(), r.direction());
    const float c = dot(oc, oc) - radius * radius;
    const float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    }

    auto recordHit = [&](float t) {
        hr.t = t;
        hr.p = r.point_at_parameter(t);
        const vec3 outwardNormal = (hr.p - currentCenter) / radius;
        hr.setFaceNormal(r, outwardNormal);
        hr.matPtr = matPtr;
        getSphereUV(outwardNormal, hr.u, hr.v);
    };

    float temp = (-b - sqrt(discriminant)) / (2.0 * a);
    if (temp > tMin && temp < tMax) {
        recordHit(temp);
        return true;
    }
    temp = (-b + sqrt(discriminant)) / (2.0 * a);
    if (temp > tMin && temp < tMax) {
        recordHit(temp);
        return true;
    }
    return false;
}

inline vec3 Sphere::centroid() const {
    // Motion blur: midpoint of the shutter path, not the union-AABB center.
    return 0.5 * (center.point_at_parameter(0) + center.point_at_parameter(1));
}

inline AABB Sphere::boundingBox() const {
    const vec3 r(radius, radius, radius);
    const AABB at0(center.point_at_parameter(0) - r, center.point_at_parameter(0) + r);
    const AABB at1(center.point_at_parameter(1) - r, center.point_at_parameter(1) + r);
    return AABB(at0, at1);
}



#endif
