#pragma once

#include "hitable.h"

/**
 * \brief Abstract material that scatters an incoming ray.
 */
class Material {
public:
    PATHTRACER_HD virtual ~Material() = default;

    /**
     * \brief Produces a scattered ray and attenuation at a hit point.
     * \param rIn incoming ray
     * \param hr surface hit
     * \param attenuation color multiplier for the scattered ray
     * \param scattered outgoing ray written on success
     * \returns false when the ray is fully absorbed
     */
    PATHTRACER_HD virtual bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation,
                                       Ray& scattered) const = 0;

    /**
     * \brief returns black as a base class
     */
    PATHTRACER_HD virtual vec3 emit(float /*u*/, float /*v*/, const vec3& /*p*/) const {
        return vec3(0, 0, 0);
    }
};
