#pragma once

#include "hitable.h"
#include "material.h"
#include "my_random.h"
#include "texture.h"

#include <memory>

class Isotropic : public Material {
public:
    Isotropic(const vec3& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
    Isotropic(std::shared_ptr<Texture> tex) : tex(std::move(tex)) {}

    bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const override {
        scattered = Ray(hr.p, randomInSphere(), rIn.time());
        attenuation = tex->value(hr.u, hr.v, hr.p);
        return true;
    }

private:
    std::shared_ptr<Texture> tex;
};
