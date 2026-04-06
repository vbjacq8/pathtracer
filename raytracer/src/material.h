#pragma once
#include"hitable.h"

class Material{
    public:
        virtual bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const = 0;
};