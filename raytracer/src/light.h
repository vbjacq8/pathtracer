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

        bool emit() const override{
            return true;
        }

        vec3 emitted() const override {
            return emission;
        }

    private: 
        vec3 emission;
};
