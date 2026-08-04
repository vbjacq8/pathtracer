#pragma once

#include "cuda_annot.h"
#include "material.h"
#include "vec3.h"

/**
 * Light source material where scatter returns false; adds emission to ray-traced recursion
 */
class Light : public Material {
public:
    PATHTRACER_HD Light(vec3 e) : emission(e) {}

    PATHTRACER_HD bool scatter(const Ray&, const HitRecord&, vec3&, Ray&) const override {
        return false;
    }

    PATHTRACER_HD vec3 emit(float /*u*/, float /*v*/, const vec3& /*p*/) const override {
        return emission;
    }

private:
    vec3 emission;
};
