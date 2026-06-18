#pragma once

#include "hitable.h"

/**
 * \brief Abstract material that scatters an incoming ray.
 */
class Material {
public:
    /**
     * \brief Produces a scattered ray and attenuation at a hit point.
     * \param rIn incoming ray
     * \param hr surface hit
     * \param attenuation color multiplier for the scattered ray
     * \param scattered outgoing ray written on success
     * \returns false when the ray is fully absorbed
     */
    virtual bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const = 0;
};
