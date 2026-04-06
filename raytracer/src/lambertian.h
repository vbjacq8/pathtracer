#pragma once
#include"material.h"
#include"my_random.h"


class Lambertian : public Material {
    public:
        Lambertian(const vec3& a): albedo(a) {}
        bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const override {
            (void) rIn;
            vec3 target = hr.p + hr.normal + randomInSphere();
            scattered = Ray(hr.p, target - hr.p);
            attenuation = albedo;
            return true;

        }
    vec3 albedo;

};