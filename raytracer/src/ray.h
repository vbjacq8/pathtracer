#ifndef RAY_H
#define RAY_H

#include "vec3.h"

/**
 * \brief Ray defined by an origin and direction.
 */
class Ray {
public:
    PATHTRACER_HD Ray() {}
    PATHTRACER_HD Ray(const vec3& a, const vec3& b, float T) : A(a), B(b), T(T) {}
    PATHTRACER_HD Ray(const vec3& a, const vec3& b) : Ray(a,b,0) {}

    /** \returns Ray origin. */
    PATHTRACER_HD vec3 origin() const { return A; }
    /** \returns Ray direction (not necessarily unit length). */
    PATHTRACER_HD vec3 direction() const { return B; }
    
    PATHTRACER_HD float time() const {return T;}
    /**
     * \brief Evaluates the ray at parameter \p t.
     * \returns \p origin + t * direction
     */
    PATHTRACER_HD vec3 point_at_parameter(float t) const { return A + B * t; }

private:
    vec3 A;
    vec3 B;
    float T;
};

/**
 * \brief Miss-ray background strategy: radiance when a ray hits no geometry.
 * \sa colorBlueWhiteGradient, colorVoid
 */
using BackgroundFn = vec3 (*)(const Ray&);

/**
 * \brief Sky color for rays that miss geometry.
 * \returns Blue-white gradient from the ray direction.
 */
PATHTRACER_HD inline vec3 colorBlueWhiteGradient(const Ray& r) {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5f * (unit_direction.y() + 1.0f);
    return (1.0f - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

/** \brief Black background for rays that miss geometry. */
PATHTRACER_HD inline vec3 colorVoid(const Ray&) {
    return vec3(0, 0, 0);
}

/**
 * \brief Ray-sphere intersection test.
 * \returns Hit distance along the ray, or -1 when there is no hit.
 */
PATHTRACER_HD inline float hitSphere(const vec3& center, float radius, const Ray& r) {
    vec3 oc = r.origin() - center;
    float b = 2.0f * dot(oc, r.direction());
    float a = dot(r.direction(), r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) {
        return -1.0f;
    } else {
        return (-b - sqrt(discriminant)) / (2.0f * a);
    }
}

/** \brief Flat color when a ray hits a sphere; sky gradient otherwise. */
PATHTRACER_HD inline vec3 colorSphere(const Ray& r, const vec3& center, float radius, const vec3& color) {
    if (hitSphere(center, radius, r) > 0.0) {
        return color;
    }
    return colorBlueWhiteGradient(r);
}

/** \brief Normal-based shading when a ray hits a sphere; sky gradient otherwise. */
PATHTRACER_HD inline vec3 colorShadedSphere(const Ray& r, const vec3& center, float radius, const vec3& color) {
    float t = hitSphere(center, radius, r);
    if (t > 0.0f) {
        vec3 N = r.point_at_parameter(t) - center;
        vec3 n = unit_vector(N * color);
        return vec3(n.x() + 1.0f, n.y() + 1.0f, n.z() + 1.0f) * 0.5f;
    }
    return colorBlueWhiteGradient(r);
}

/** \brief Normal-based shading when a ray hits a sphere; sky gradient otherwise. */
PATHTRACER_HD inline vec3 colorShadedSphere(const Ray& r, const vec3& center, float radius) {
    float t = hitSphere(center, radius, r);
    if (t > 0.0f) {
        vec3 N = unit_vector(r.point_at_parameter(t) - center);
        return vec3(N.x() + 1.0f, N.y() + 1.0f, N.z() + 1.0f) * 0.5f;
    }
    return colorBlueWhiteGradient(r);
}

#endif
