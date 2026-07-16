#ifndef SPHERE_H
#define SPHERE_H

#include "hitable.h"
#include "material.h"
#include <numbers>

constexpr double PI = std::numbers::pi;

/**
 * \brief Sphere primitive with an attached material.
 */
class Sphere : public Hitable {
public:
    Sphere() {}

    Sphere(const vec3& staticCenter, double R, MaterialPtr mat)
        : center(staticCenter, vec3(0, 0, 0)), radius(R), matPtr(std::move(mat)) {}

    Sphere(const vec3& center1, const vec3& center2, double R, MaterialPtr mat)
        : center(center1, center2 - center1), radius(R), matPtr(std::move(mat)) {}

    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override;
    AABB boundingBox() const override;
    vec3 centroid() const override;
    

private:
    Ray center;
    double radius;
    MaterialPtr matPtr;

    /**
     * \brief Maps a unit-sphere direction to texture (u, v) in [0, 1].
     * \param p unit vector from sphere center to the hit (same as outward normal)
     * \param u azimuth in [0, 1], starting at -x
     * \param v zenith in [0, 1], starting at -y
     */
    static void getSphereUV(const vec3& p, double& u, double& v) {
        // p must be a unit vector; acos domain is [-1, 1].
        const double theta = std::acos(-p.y());
        const double phi = std::atan2(-p.z(), p.x()) + PI;
        u = phi / (2 * PI);
        v = theta / PI;
    }
    
};

/**
 * \brief Ray-sphere intersection for Sphere.
 * \copydoc Hitable::hit
 */
inline bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& hr) {
    // Use the *incoming* ray's shutter time, not center.time() (which is always 0).
    vec3 currentCenter = center.point_at_parameter(r.time());
    vec3 oc = r.origin() - currentCenter;
    double b = 2.0 * dot(oc, r.direction());
    double a = dot(r.direction(), r.direction());
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    }
    double temp = (-b - sqrt(discriminant)) / (2.0 * a);
    if (temp > tMin && temp < tMax) {
        hr.t = temp;
        hr.p = r.point_at_parameter(temp);
        hr.normal = (hr.p - currentCenter) / radius;
        hr.matPtr = matPtr;
        getSphereUV(hr.normal, hr.u, hr.v);
        return true;
    }
    temp = (-b + sqrt(discriminant)) / (2.0 * a);
    if (temp > tMin && temp < tMax) {
        hr.t = temp;
        hr.p = r.point_at_parameter(temp);
        hr.normal = (hr.p - currentCenter) / radius;
        hr.matPtr = matPtr;
        getSphereUV(hr.normal, hr.u, hr.v);
        return true;
    }
    return false;
}

inline vec3 Sphere::centroid() const {
    // Midpoint of the shutter interval (t = 0 and t = 1).
    return 0.5 * (center.point_at_parameter(0) + center.point_at_parameter(1));
}

inline AABB Sphere::boundingBox() const {
    const vec3 r(radius, radius, radius);
    const AABB at0(center.point_at_parameter(0) - r, center.point_at_parameter(0) + r);
    const AABB at1(center.point_at_parameter(1) - r, center.point_at_parameter(1) + r);
    return AABB(at0, at1);
}



#endif
