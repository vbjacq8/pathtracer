#pragma once

#include "../camera.h"
#include "../parse.h"
#include "../vec3.h"

#include <cmath>

/** \brief Rotates \p v around unit axis \p axis by \p angle radians. */
inline vec3 rotateAroundAxis(const vec3& v, const vec3& axis, float angle) {
    vec3 k = unit_vector(axis);
    float c = std::cos(angle);
    float s = std::sin(angle);
    return v * c + cross(k, v) * s + k * dot(k, v) * (1.0 - c);
}

/** \returns Pixel height derived from \p opts width, height, and aspect. */
inline int renderHeight(const RenderOptions& opts) {
    if (opts.height > 0) {
        return opts.height;
    }
    return static_cast<int>(opts.width / opts.aspect);
}

/** \returns Width / height for \p opts. */
inline float renderAspect(const RenderOptions& opts) {
    const int ny = renderHeight(opts);
    return float(opts.width) / float(ny);
}

/** \brief Builds a Camera from render options. */
inline Camera makeCamera(const RenderOptions& opts) {
    // Keep focus on the lookat point so orbit/pan/fly don't leave everything defocused.
    const float focusDist = (opts.lookfrom - opts.lookat).norm();
    return Camera(opts.lookfrom, opts.lookat, opts.vup, opts.vfov,
                  renderAspect(opts), opts.aperture, focusDist > 1e-6 ? focusDist : opts.focusDist);
}

/**
 * \brief Orbits \p lookfrom around \p lookat.
 * \param dx yaw delta in radians (caller scales from pixels)
 * \param dy pitch delta in radians
 */
inline void orbitCamera(RenderOptions& opts, float dx, float dy) {
    vec3 offset = opts.lookfrom - opts.lookat;
    if (offset.squared_norm() < 1e-12) {
        return;
    }

    offset = rotateAroundAxis(offset, opts.vup, -dx);

    vec3 forward = unit_vector(-offset);
    vec3 right = unit_vector(cross(opts.vup, forward));
    offset = rotateAroundAxis(offset, right, dy);

    opts.lookfrom = opts.lookat + offset;
}

/** \brief Moves \p lookfrom toward or away from \p lookat by \p delta. */
inline void dollyCamera(RenderOptions& opts, float delta) {
    vec3 dir = unit_vector(opts.lookfrom - opts.lookat);
    float dist = (opts.lookfrom - opts.lookat).norm();
    dist = std::max(0.1f, dist + delta);
    opts.lookfrom = opts.lookat + dir * dist;
}

/** \brief Translates both \p lookfrom and \p lookat in the view plane. */
inline void panCamera(RenderOptions& opts, float dx, float dy) {
    vec3 forward = unit_vector(opts.lookat - opts.lookfrom);
    vec3 right = unit_vector(cross(opts.vup, forward));
    vec3 up = cross(forward, right);
    vec3 delta = (-dx * right) + (dy * up);
    opts.lookfrom += delta;
    opts.lookat += delta;
}

/** \brief Moves \p lookfrom and \p lookat along the view forward and right axes. */
inline void flyCamera(RenderOptions& opts, float forwardAmt, float rightAmt) {
    vec3 forward = unit_vector(opts.lookat - opts.lookfrom);
    vec3 right = unit_vector(-1 * cross(opts.vup, forward));
    vec3 delta = forward * forwardAmt + right * rightAmt;
    opts.lookfrom += delta;
    opts.lookat += delta;
}
