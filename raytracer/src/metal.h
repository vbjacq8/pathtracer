#pragma once

#include "material.h"
#include "my_random.h"

/**
 * \brief Reflective metal with optional surface fuzz.
 */
class Metal : public Material {
public:
    Metal(const vec3& a, float f) : albedo(a) {
        if (f < 0) {
            fuzz = 0;
        } else if (f < 1) {
            fuzz = f;
        } else {
            fuzz = 1;
        }
    }

    /**
     * \brief Reflects the ray with optional perturbation; rejects grazing hits.
     * \copydoc Material::scatter
     */
    bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const override {
        vec3 reflected = reflect(unit_vector(rIn.direction()), hr.normal);
        scattered = Ray(hr.p, reflected + fuzz * randomInSphere());
        attenuation = albedo;
        return (dot(scattered.direction(), hr.normal) > 0);
    }

    vec3 reflect(const vec3& v, const vec3& n) const {
        return v - 2 * dot(v, n) * n;
    }

    vec3 albedo;
    float fuzz;
};
