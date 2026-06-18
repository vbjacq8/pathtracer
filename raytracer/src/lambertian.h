#pragma once

#include "material.h"
#include "my_random.h"

/**
 * \brief Ideal diffuse (Lambertian) material.
 */
class Lambertian : public Material {
public:
    Lambertian(const vec3& a) : albedo(a) {}

    /**
     * \brief Scatters with a random direction in the hemisphere around the normal.
     * \copydoc Material::scatter
     */
    bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const override {
        (void)rIn;
        vec3 target = hr.p + hr.normal + randomInSphere();
        scattered = Ray(hr.p, target - hr.p);
        attenuation = albedo;
        return true;
    }

    vec3 albedo;
};
