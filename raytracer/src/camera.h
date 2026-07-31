#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "constants.h"
#include "my_random.h"

/**
 * \brief Pinhole or thin-lens camera that generates primary rays through a viewport.
 */
class Camera {
public:
    Camera(const vec3& lookfrom, const vec3& lookat, const vec3& vup, float vfov, float aspect, float aperture, float focusDist) {
        this->lensRadius = aperture / 2;
        this->focusDistance = focusDist;
        float theta = vfov * pi / 180;
        float halfHeight = tan(theta / 2) * focusDist;
        float halfWidth = halfHeight * aspect;
        this->w = unit_vector(lookfrom - lookat);
        this->u = unit_vector(cross(vup, w));
        this->v = cross(w, u);
        this->horizontal = 2 * halfWidth * u;
        this->vertical = 2 * halfHeight * v;
        this->origin = lookfrom;
        this->lowerLeftCorner = origin - halfWidth * u - halfHeight * v - w * focusDist;
    }

    Camera(const vec3& lookfrom, const vec3& lookat, const vec3& vup, float vfov, float aspect) {
        float theta = vfov * pi / 180;
        float halfHeight = tan(theta / 2);
        float halfWidth = halfHeight * aspect;
        this->w = unit_vector(lookfrom - lookat);
        this->u = unit_vector(cross(vup, w));
        this->v = cross(w, u);
        this->horizontal = 2 * halfWidth * u;
        this->vertical = 2 * halfHeight * v;
        this->origin = lookfrom;
        this->lowerLeftCorner = origin - halfWidth * u - halfHeight * v - w;
        this->lensRadius = 0;
        this->focusDistance = 1;
    }

    Camera(float vfov, float aspect) {
        float theta = vfov * pi / 180;
        float halfHeight = tan(theta / 2);
        float halfWidth = halfHeight * aspect;
        this->horizontal = vec3(2 * halfWidth, 0, 0);
        this->vertical = vec3(0, 2 * halfHeight, 0);
        this->lowerLeftCorner = vec3(-halfWidth, -halfHeight, -1);
        this->origin = vec3(0, 0, 0);
    }

    /** Legacy constructor for explicit viewport geometry. */
    Camera(const vec3& lowerLeftCorner, const vec3& horizontal, const vec3& vertical, const vec3& origin) {
        this->lowerLeftCorner = lowerLeftCorner;
        this->vertical = vertical;
        this->horizontal = horizontal;
        this->origin = origin;
    }

    /**
     * \brief Generates a primary ray through normalized viewport coordinates.
     * \param s horizontal coordinate in [0, 1]
     * \param t vertical coordinate in [0, 1]
     * \returns Ray from the lens or pinhole through the viewport
     */
    Ray getRay(float s, float t) const {
        vec3 rd = lensRadius * randomInDisc();
        vec3 offset = rd.x() * u + rd.y() * v;
        float rayTime = randomFloat(0,1);
        return Ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset, rayTime);
    }

    /**
     * \brief Supersamples one pixel with \p numSamples jittered rays.
     * \returns Averaged radiance for pixel (\p i, \p j)
     */
    vec3 colorSample(int i, int j, int nx, int ny, int numSamples, Hitable* world, int depth,
                     BackgroundFn background = colorBlueWhiteGradient) {
        vec3 col(0, 0, 0);
        for (int k = 0; k < numSamples; k++) {
            float randcoeff1 = randomFloat(0.0, 1.0);
            float randcoeff2 = randomFloat(0.0, 1.0);
            float u = float(i + randcoeff1) / float(nx);
            float v = float(j + randcoeff2) / float(ny);
            Ray r = getRay(u, v);
            col += pathTrace(r, world, depth, background);
        }
        col /= numSamples;
        return col;
    }

    vec3 origin = vec3(0, 0, 0);
    vec3 lowerLeftCorner = vec3(0, 0, -1);
    vec3 vertical = vec3(0, 2, 0);
    vec3 horizontal = vec3(2, 0, 0);
    vec3 u = vec3(1, 0, 0);
    vec3 v = vec3(0, 1, 0);
    vec3 w = vec3(0, 0, 1);
    float lensRadius = 0.0;
    float focusDistance = 1.0;
};

#endif
