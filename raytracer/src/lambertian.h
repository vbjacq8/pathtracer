#pragma once

#include "material.h"
#include "my_random.h"
#include "texture.h"

/**
 * \brief Ideal diffuse (Lambertian) material.
 *
 * \p tex is non-owning (scene / device texture pool owns the texture).
 */
class Lambertian : public Material {
public:
    /** \brief Solid-color convenience: wraps \p a in a SolidColor texture (caller must track it). */
    PATHTRACER_HD Lambertian(const vec3& a) : tex(new SolidColor(a)) {}

    PATHTRACER_HD Lambertian(Texture* texture) : tex(texture) {}

    Lambertian(const Lambertian&) = delete;
    Lambertian& operator=(const Lambertian&) = delete;

    /**
     * \brief Scatters with a random direction in the hemisphere around the normal.
     * \copydoc Material::scatter
     */
    PATHTRACER_HD bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation,
                               Ray& scattered) const override {
        vec3 newDirection = hr.normal + randomInSphere();
        if (newDirection.near_zero()) {
            newDirection = hr.normal;
        }
        scattered = Ray(hr.p, newDirection, rIn.time());
        attenuation = tex->value(hr.u, hr.v, hr.p);
        return true;
    }

    Texture* tex;
};
