#pragma once

#include "cuda_annot.h"
#include "hitable.h"
#include "material.h"
#include "my_random.h"
#include "texture.h"

/**
 * \brief Volume phase function.
 *
 * \p tex is non-owning (scene / device texture pool owns the texture).
 */
class Isotropic : public Material {
public:
    PATHTRACER_HD Isotropic(const vec3& albedo) : tex(new SolidColor(albedo)) {}
    PATHTRACER_HD Isotropic(Texture* texture) : tex(texture) {}

    Isotropic(const Isotropic&) = delete;
    Isotropic& operator=(const Isotropic&) = delete;

    PATHTRACER_HD bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation,
                               Ray& scattered) const override {
        scattered = Ray(hr.p, randomInSphere(), rIn.time());
        attenuation = tex->value(hr.u, hr.v, hr.p);
        return true;
    }

private:
    Texture* tex;
};
