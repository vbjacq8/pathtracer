#pragma once

#include "material.h"
#include "my_random.h"
#include "texture.h"

#include <memory>

/**
 * \brief Ideal diffuse (Lambertian) material.
 */
class Lambertian : public Material {
public:
    /** \brief Solid-color convenience: wraps \p a in a SolidColor texture. */
    Lambertian(const vec3& a) : tex(std::make_shared<SolidColor>(a)) {}

    Lambertian(std::shared_ptr<Texture> texture) : tex(std::move(texture)) {}

    /**
     * \brief Scatters with a random direction in the hemisphere around the normal.
     * \copydoc Material::scatter
     */
    bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const override {
        vec3 newDirection = hr.normal + randomInSphere();
        if (newDirection.near_zero()) {
            newDirection = hr.normal;
        }
        scattered = Ray(hr.p, newDirection, rIn.time());
        attenuation = tex->value(hr.u, hr.v, hr.p);
        return true;
    }

    std::shared_ptr<Texture> tex;
};
