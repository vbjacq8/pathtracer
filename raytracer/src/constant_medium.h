#pragma once

#include"material.h"
#include"hitable.h"
#include"texture.h"
#include"constants.h"
#include"isotropic.h"

class ConstantMedium : public Hitable {
    public:
        ConstantMedium(std::shared_ptr<Hitable> boundary, float density, std::shared_ptr<Texture> tex) :
            boundary(boundary), negInvDensity(-1/density), phaseFunction(std::make_shared<Isotropic>(tex)) {}

        ConstantMedium(std::shared_ptr<Hitable> boundary, float density, const vec3& albedo) :
        boundary(boundary), negInvDensity(-1/density), phaseFunction(std::make_shared<Isotropic>(albedo)) {}

        bool hit(const Ray& rIn, float tMin, float tMax, HitRecord& hr) override {
            HitRecord hr1, hr2;
            if (!boundary->hit(rIn, -infinity, infinity, hr1)){return false;}
            if (!boundary->hit(rIn, hr1.t+ 0.001, infinity, hr2)){return false;}

            if (hr1.t < tMin){hr1.t = tMin;}
            if (hr2.t > tMax){hr2.t = tMax;}

            if (hr1.t >= hr2.t){return false;}

            if (hr1.t < 0){hr1.t = 0;}

            float rayLength = rIn.direction().norm();
            float distanceInBoundary = rayLength * (hr2.t - hr1.t);
            float hitDistance = negInvDensity * std::log(randomFloat(0, 1));

            if (hitDistance > distanceInBoundary){return false;}

            hr.t = hr1.t + hitDistance / rayLength;
            hr.p = rIn.point_at_parameter(hr.t);

            hr.normal = vec3(1,0,0); //arbitrary
            hr.frontFace = true; //arbitrary
            hr.matPtr = phaseFunction;
            return true;
        }

        AABB boundingBox() const override {return boundary->boundingBox();}

    private:
        std::shared_ptr<Hitable> boundary;
        float negInvDensity;
        std::shared_ptr<Material> phaseFunction;
};