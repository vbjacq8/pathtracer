#pragma once

#include "hittables.h"
#include "my_random.h"

/**
 * \brief Schlick approximation for Fresnel reflectance.
 * \param cosine cosine of the angle between the incident ray and the normal
 * \param nt ratio of indices of refraction
 */
float schlick(float cosine, float nt) {
    float r0 = (1 - nt) / (1 + nt);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

/**
 * \brief Dielectric material with reflection and refraction.
 */
class Dielectric : public Material {
public:
    Dielectric(float ri) : nt(ri) {}

    /**
     * \brief Reflects or refracts based on Snell's law and Schlick's approximation.
     * \copydoc Material::scatter
     */
    virtual bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const override {
        vec3 outwardNormal;
        float R;
        vec3 v = rIn.direction();
        vec3 refracted;
        attenuation = vec3(1.0, 1.0, 1.0);
        float cosine;
        if (dot(v, hr.normal) < 0) {
            R = 1 / nt;
            outwardNormal = hr.normal;
            cosine = -dot(v, hr.normal) / v.norm();
        } else {
            outwardNormal = -1 * hr.normal;
            cosine = nt * dot(v, hr.normal) / v.norm();
            R = nt;
        }
        vec3 reflected = reflect(v, outwardNormal);

        if (refract(v, outwardNormal, R, refracted)) {
            if (randomDouble(0, 1.0) < schlick(cosine, nt)) {
                scattered = Ray(hr.p, reflected, rIn.time());
            } else {
                scattered = Ray(hr.p, refracted, rIn.time());
            }
        } else {
            scattered = Ray(hr.p, reflected,rIn.time());
        }
        return true;
    }

    vec3 reflect(const vec3& v, const vec3& n) const {
        return v - 2 * dot(v, n) * n;
    }

    /**
     * \brief Refracts \p v across the boundary with normal \p n.
     * \param v incident direction
     * \param n outward surface normal
     * \param R ratio of indices of refraction
     * \param refracted refracted direction written on success
     * \returns false on total internal reflection
     */
    bool refract(const vec3& v, const vec3& n, float R, vec3& refracted) const {
        vec3 uv = unit_vector(v);
        float dt = dot(uv, n);
        float D = 1 - R * R * (1 - dt * dt);
        if (D > 0) {
            refracted = R * (uv - n * dt) - n * sqrt(D);
            return true;
        } else {
            return false;
        }
    }

    vec3 attenuation;
    float nt;
};
