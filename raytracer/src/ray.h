#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/**
 * \brief Ray defined by an origin and direction.
 */
class Ray {
public:
    Ray() {}
    Ray(const vec3& a, const vec3& b) : A(a), B(b) {}

    /** \returns Ray origin. */
    vec3 origin() const { return A; }
    /** \returns Ray direction (not necessarily unit length). */
    vec3 direction() const { return B; }
    /**
     * \brief Evaluates the ray at parameter \p t.
     * \returns \p origin + t * direction
     */
    vec3 point_at_parameter(double t) const { return A + B * t; }

private:
    vec3 A;
    vec3 B;
};

/**
 * \brief Sky color for rays that miss geometry.
 * \returns Blue-white gradient from the ray direction.
 */
inline vec3 colorBlueWhiteGradient(const Ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

/**
 * \brief Ray-sphere intersection test.
 * \returns Hit distance along the ray, or -1 when there is no hit.
 */
inline double hitSphere(const vec3& center, double radius, const Ray& r) {
    vec3 oc = r.origin() - center;
    double b = 2.0 * dot(oc, r.direction());
    double a = dot(r.direction(), r.direction());
    double c = dot(oc, oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-b - sqrt(discriminant)) / (2 * a);
    }
}

/** \brief Flat color when a ray hits a sphere; sky gradient otherwise. */
inline vec3 colorSphere(const Ray& r, const vec3& center, double radius, const vec3& color) {
    if (hitSphere(center, radius, r) > 0.0) {
        return color;
    }
    return colorBlueWhiteGradient(r);
}

/** \brief Normal-based shading when a ray hits a sphere; sky gradient otherwise. */
inline vec3 colorShadedSphere(const Ray& r, const vec3& center, double radius, const vec3& color) {
    double t = hitSphere(center, radius, r);
    if (t > 0.0) {
        vec3 N = r.point_at_parameter(t) - center;
        vec3 n = unit_vector(N * color);
        return vec3(n.x() + 1, n.y() + 1, n.z() + 1) * 0.5;
    }
    return colorBlueWhiteGradient(r);
}

/** \brief Normal-based shading when a ray hits a sphere; sky gradient otherwise. */
inline vec3 colorShadedSphere(const Ray& r, const vec3& center, double radius) {
    double t = hitSphere(center, radius, r);
    if (t > 0.0) {
        vec3 N = unit_vector(r.point_at_parameter(t) - center);
        return vec3(N.x() + 1, N.y() + 1, N.z() + 1) * 0.5;
    }
    return colorBlueWhiteGradient(r);
}

#endif
