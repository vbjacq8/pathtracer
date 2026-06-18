#pragma once

#include "hitable_list.h"
#include <float.h>
#include "my_random.h"

/**
 * Path-tracing integrators. Each function takes ray \p r, scene \p world,
 * bounce limit \p depth, and recursion index \p i.
 */

/**
 * \brief Debug shading from surface normals.
 * \returns Normal map color on hit; sky gradient on miss.
 */
inline vec3 color(const Ray& r, Hitable* world, int depth, int i = 0) {
    (void)depth;
    (void)i;
    HitRecord hr;
    if (world->hit(r, 0.001, MAXFLOAT, hr)) {
        return 0.5 * vec3(hr.normal.x() + 1.0, hr.normal.y() + 1.0, hr.normal.z() + 1.0);
    }
    return colorBlueWhiteGradient(r);
}

/**
 * \brief Diffuse path tracing through Lambertian surfaces.
 * \returns Attenuated recursive color on hit; sky gradient on miss.
 */
inline vec3 diffuseColor(const Ray& r, Hitable* world, int depth, int i = 0) {
    if (i >= depth) {
        return vec3(0, 0, 0);
    }
    HitRecord hr;
    if (world->hit(r, 0.001, MAXFLOAT, hr)) {
        Ray scattered;
        vec3 attenuation;
        if (hr.matPtr && hr.matPtr->scatter(r, hr, attenuation, scattered)) {
            return attenuation * diffuseColor(scattered, world, depth, i + 1);
        } else {
            throw std::runtime_error("scatter returned false");
        }
    } else {
        return colorBlueWhiteGradient(r);
    }
}

/**
 * \brief Specular path tracing through metal and dielectric surfaces.
 * \returns Attenuated recursive color on hit; sky gradient on miss.
 */
inline vec3 metalColor(const Ray& r, Hitable* world, int depth, int i = 0) {
    if (i >= depth) {
        return vec3(0, 0, 0);
    }
    HitRecord hr;
    if (world->hit(r, 0.001, MAXFLOAT, hr)) {
        Ray scattered;
        vec3 attenuation;
        if (hr.matPtr && hr.matPtr->scatter(r, hr, attenuation, scattered)) {
            return attenuation * metalColor(scattered, world, depth, i + 1);
        } else {
            if (!hr.matPtr) {
                throw std::runtime_error("null matPtr");
            }
            return vec3(0, 0, 0);
        }
    } else {
        return colorBlueWhiteGradient(r);
    }
}

/**
 * \brief Path tracing with a wider hit tolerance for thin dielectric shells.
 * \returns Attenuated recursive color on hit; sky gradient on miss.
 */
inline vec3 dielectricColor(const Ray& r, Hitable* world, int depth, int i = 0) {
    if (i >= depth) {
        return vec3(0, 0, 0);
    }
    HitRecord hr;
    if (world->hit(r, 0.005, MAXFLOAT, hr)) {
        Ray scattered;
        vec3 attenuation;
        if (hr.matPtr && hr.matPtr->scatter(r, hr, attenuation, scattered)) {
            return attenuation * dielectricColor(scattered, world, depth, i + 1);
        } else {
            if (!hr.matPtr) {
                throw std::runtime_error("null matPtr");
            }
            return vec3(0, 0, 0);
        }
    } else {
        return colorBlueWhiteGradient(r);
    }
}
