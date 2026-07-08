#ifndef SPHERE_H
#define SPHERE_H

#include "hitable.h"
#include "material.h"

/**
 * \brief Sphere primitive with an attached material.
 */
class Sphere : public Hitable {
public:
    Sphere() {}
    Sphere(const vec3& cen, double R, Material* const mat) : center(cen), radius(R), matPtr(mat) {}
    virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override;
    AABB boundingBox() const override;
    vec3 centroid() const override;

    vec3 center;
    double radius;
    Material* matPtr;
};

/**
 * \brief Ray-sphere intersection for Sphere.
 * \copydoc Hitable::hit
 */
inline bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& hr) {
    vec3 oc = r.origin() - center;
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
        hr.normal = (r.point_at_parameter(temp) - center) / radius;
        hr.matPtr = matPtr;
        return true;
    }
    temp = (-b + sqrt(discriminant)) / (2.0 * a);
    if (temp > tMin && temp < tMax) {
        hr.t = temp;
        hr.p = r.point_at_parameter(temp);
        hr.normal = (r.point_at_parameter(temp) - center) / radius;
        hr.matPtr = matPtr;
        return true;
    }
    return false;
}

inline vec3 Sphere::centroid() const { return center; }

inline AABB Sphere::boundingBox() const {
    const vec3 r(radius, radius, radius);
    AABB box;
    box.min = center - r;
    box.max = center + r;
    return box;
}

#endif
