#pragma once

#include "constants.h"
#include "hitable.h"
#include "isotropic.h"
#include "material.h"
#include "my_random.h"
#include "texture.h"

#include <cmath>
#include <memory>

/**
 * \brief Participating medium. Owns \p phaseFunction; boundary is shared.
 *
 * Albedo ctor also owns a SolidColor. Texture* ctor borrows \p tex (caller owns it).
 */
class ConstantMedium : public Hitable {
public:
    ConstantMedium(std::shared_ptr<Hitable> boundary, float density, Texture* tex)
        : boundary(std::move(boundary))
        , negInvDensity(-1 / density)
        , ownedTex(nullptr)
        , phaseFunction(new Isotropic(tex)) {}

    ConstantMedium(std::shared_ptr<Hitable> boundary, float density, const vec3& albedo)
        : boundary(std::move(boundary))
        , negInvDensity(-1 / density)
        , ownedTex(new SolidColor(albedo))
        , phaseFunction(new Isotropic(ownedTex)) {}

    ~ConstantMedium() override {
        delete phaseFunction;
        delete ownedTex;
    }

    ConstantMedium(const ConstantMedium&) = delete;
    ConstantMedium& operator=(const ConstantMedium&) = delete;

    PATHTRACER_HD bool hit(const Ray& rIn, float tMin, float tMax, HitRecord& hr) override {
        HitRecord hr1, hr2;
        if (!boundary->hit(rIn, -infinity, infinity, hr1)) {
            return false;
        }
        if (!boundary->hit(rIn, hr1.t + 0.001f, infinity, hr2)) {
            return false;
        }

        if (hr1.t < tMin) {
            hr1.t = tMin;
        }
        if (hr2.t > tMax) {
            hr2.t = tMax;
        }

        if (hr1.t >= hr2.t) {
            return false;
        }

        if (hr1.t < 0) {
            hr1.t = 0;
        }

        float rayLength = rIn.direction().norm();
        float distanceInBoundary = rayLength * (hr2.t - hr1.t);
        float hitDistance = negInvDensity * logf(randomFloat(0, 1));

        if (hitDistance > distanceInBoundary) {
            return false;
        }

        hr.t = hr1.t + hitDistance / rayLength;
        hr.p = rIn.point_at_parameter(hr.t);

        hr.normal = vec3(1, 0, 0);  // arbitrary
        hr.frontFace = true;       // arbitrary
        hr.matPtr = phaseFunction;
        return true;
    }

    PATHTRACER_HD AABB boundingBox() const override { return boundary->boundingBox(); }

private:
    std::shared_ptr<Hitable> boundary;
    float negInvDensity;
    Texture* ownedTex;
    Material* phaseFunction;
};
