#pragma once

#include"material.h"
#include"vec3.h"

/**
 * Light source material where scatter returns false; adds emission to ray-traced recursion
 */
class Light : public Material {
    public: 
        Light(vec3 e) : emission(e) {}

        bool scatter(const Ray&, const HitRecord&, vec3&, Ray&) const override {
            return false;
        }

        vec3 emit(double u, double v, const vec3& p) const override{
            return emission;
        }

    private: 
        vec3 emission;
};
