#pragma once

#include "hitable.h"

#include <memory>

/**
 * \brief Abstract material that scatters an incoming ray.
 */
class Material {
public:
    virtual ~Material() = default;

    /**
     * \brief Produces a scattered ray and attenuation at a hit point.
     * \param rIn incoming ray
     * \param hr surface hit
     * \param attenuation color multiplier for the scattered ray
     * \param scattered outgoing ray written on success
     * \returns false when the ray is fully absorbed
     */
    virtual bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const = 0;

    virtual bool emit() const { return false; };

    virtual vec3 emitted() const { return vec3(0, 0, 0); };
};

using MaterialPtr = std::shared_ptr<Material>;
