#pragma once

#include "cuda_annot.h"
#include "vec3.h"

/**
 * \brief Reflects incident direction \p v about unit normal \p n.
 */
PATHTRACER_HD inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2.0f * dot(v, n) * n;
}

/**
 * \brief Refracts incident direction \p v across the boundary with normal \p n.
 * \param v incident direction
 * \param n outward surface normal (unit-ish; function renormalizes \p v)
 * \param etaRatio ratio of indices of refraction (ηᵢ / ηₜ)
 * \param refracted refracted direction written on success
 * \returns false on total internal reflection
 */
PATHTRACER_HD inline bool refract(const vec3& v, const vec3& n, float etaRatio, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float disc = 1.0f - etaRatio * etaRatio * (1.0f - dt * dt);
    if (disc > 0.0f) {
        refracted = etaRatio * (uv - n * dt) - n * sqrtf(disc);
        return true;
    }
    return false;
}

/**
 * \brief Schlick approximation for Fresnel reflectance.
 * \param cosine cosine of the angle between the incident ray and the normal
 * \param ior relative index of refraction used by the caller
 */
PATHTRACER_HD inline float schlick(float cosine, float ior) {
    float r0 = (1.0f - ior) / (1.0f + ior);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}
