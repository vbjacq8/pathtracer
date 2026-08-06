#pragma once

#include "material.h"
#include "my_random.h"
#include "optics.h"

/**
 * \brief Dielectric material with reflection and refraction.
 */
class Dielectric : public Material {
public:
    PATHTRACER_HD Dielectric(float ri) : nt(ri) {}

    /**
     * \brief Reflects or refracts based on Snell's law and Schlick's approximation.
     * \copydoc Material::scatter
     */
    PATHTRACER_HD bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation,
                               Ray& scattered) const override {
        vec3 outwardNormal;
        float etaRatio;
        vec3 v = rIn.direction();
        vec3 refracted;
        attenuation = vec3(1.0f, 1.0f, 1.0f);
        float cosine;
        if (dot(v, hr.normal) < 0) {
            etaRatio = 1.0f / nt;
            outwardNormal = hr.normal;
            cosine = -dot(v, hr.normal) / v.norm();
        } else {
            outwardNormal = -1.0f * hr.normal;
            cosine = nt * dot(v, hr.normal) / v.norm();
            etaRatio = nt;
        }
        vec3 reflected = reflect(v, outwardNormal);

        if (refract(v, outwardNormal, etaRatio, refracted)) {
            if (randomFloat(0.0f, 1.0f) < schlick(cosine, nt)) {
                scattered = Ray(hr.p, reflected, rIn.time());
            } else {
                scattered = Ray(hr.p, refracted, rIn.time());
            }
        } else {
            scattered = Ray(hr.p, reflected, rIn.time());
        }
        return true;
    }

    float nt;
};
