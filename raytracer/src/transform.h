#pragma once

#include "aabb.h"
#include "constants.h"
#include "hitable.h"

#include <cmath>
#include <memory>

/**
 * \brief Translates a child hitable by a fixed offset.
 */
class Translate : public Hitable {
public:
    Translate(HitablePtr object, const vec3& offset)
        : object(std::move(object)), offset(offset) {
        aabb = this->object->boundingBox() + offset;
    }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override {
        // Move the ray into object space (translation does not change direction).
        const Ray offsetRay(r.origin() - offset, r.direction(), r.time());
        if (!object->hit(offsetRay, tMin, tMax, hr)) {
            return false;
        }

        hr.p += offset;
        return true;
    }

    AABB boundingBox() const override { return aabb; }

private:
    HitablePtr object;
    vec3 offset;
    AABB aabb;
};

/**
 * \brief Y-axis rotation wrapper. \p angle is in degrees (clockwise about +Y).
 */
class RotateY : public Hitable {
public:
    RotateY(HitablePtr object, double angle) : object(std::move(object)) {
        const double radians = degreesToRadians(angle);
        cosTheta = std::cos(radians);
        sinTheta = std::sin(radians);
        bounds = this->object->boundingBox();

        vec3 min(infinity, infinity, infinity);
        vec3 max(-infinity, -infinity, -infinity);

        // Test all 8 AABB corners after rotation and take the enclosing box.
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    const double x = i * bounds.max[0] + (1 - i) * bounds.min[0];
                    const double y = j * bounds.max[1] + (1 - j) * bounds.min[1];
                    const double z = k * bounds.max[2] + (1 - k) * bounds.min[2];

                    const double newX = cosTheta * x + sinTheta * z;
                    const double newZ = -sinTheta * x + cosTheta * z;
                    const vec3 tester(newX, y, newZ);

                    for (int c = 0; c < 3; ++c) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bounds = AABB(min, max);
    }

    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override {
        // World -> object space (inverse rotation).
        const vec3 origin(
            cosTheta * r.origin().x() - sinTheta * r.origin().z(),
            r.origin().y(),
            sinTheta * r.origin().x() + cosTheta * r.origin().z());
        const vec3 direction(
            cosTheta * r.direction().x() - sinTheta * r.direction().z(),
            r.direction().y(),
            sinTheta * r.direction().x() + cosTheta * r.direction().z());

        const Ray rotated(origin, direction, r.time());
        if (!object->hit(rotated, tMin, tMax, hr)) {
            return false;
        }

        // Object -> world space.
        hr.p = vec3(
            cosTheta * hr.p.x() + sinTheta * hr.p.z(),
            hr.p.y(),
            -sinTheta * hr.p.x() + cosTheta * hr.p.z());
        hr.normal = vec3(
            cosTheta * hr.normal.x() + sinTheta * hr.normal.z(),
            hr.normal.y(),
            -sinTheta * hr.normal.x() + cosTheta * hr.normal.z());

        return true;
    }

    AABB boundingBox() const override { return bounds; }

private:
    HitablePtr object;
    double cosTheta = 0;
    double sinTheta = 0;
    AABB bounds;
};
