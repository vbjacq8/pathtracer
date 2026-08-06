#pragma once

#include "../../src/hitable.h"
#include "../../src/optics.h"
#include "../../src/ray.h"
#include "../../src/vec3.h"

#include "material_rec.cuh"
#include "my_random.cuh"

/**
 * \brief Flat material scatter — switch on MatType (no virtuals).
 * \returns false when the ray is absorbed (lights / failed metal).
 */
__device__ inline bool scatterMaterial(const MaterialRec& m, const TextureRec* textures,
                                       const Ray& rIn, const HitRecord& hr, vec3& attenuation,
                                       Ray& scattered, RNG* states, int tid) {
    switch (m.type) {
    case MatType::Lambertian: {
        vec3 newDirection = hr.normal + randomInSphere(states, tid);
        if (newDirection.near_zero()) {
            newDirection = hr.normal;
        }
        scattered = Ray(hr.p, newDirection, rIn.time());
        attenuation = sampleMaterialAlbedo(m, textures, hr.u, hr.v, hr.p);
        return true;
    }
    case MatType::Metal: {
        vec3 reflected = reflect(unit_vector(rIn.direction()), hr.normal);
        scattered = Ray(hr.p, reflected + m.fuzz * randomInSphere(states, tid), rIn.time());
        attenuation = m.albedo;
        return dot(scattered.direction(), hr.normal) > 0.0f;
    }
    case MatType::Dielectric: {
        vec3 outwardNormal;
        float etaRatio;
        const vec3 v = rIn.direction();
        vec3 refracted;
        attenuation = vec3(1.0f, 1.0f, 1.0f);
        float cosine;
        if (dot(v, hr.normal) < 0.0f) {
            etaRatio = 1.0f / m.ior;
            outwardNormal = hr.normal;
            cosine = -dot(v, hr.normal) / v.norm();
        } else {
            outwardNormal = -1.0f * hr.normal;
            cosine = m.ior * dot(v, hr.normal) / v.norm();
            etaRatio = m.ior;
        }
        const vec3 reflected = reflect(v, outwardNormal);
        if (refract(v, outwardNormal, etaRatio, refracted)) {
            if (randomFloat(0.0f, 1.0f, states, tid) < schlick(cosine, m.ior)) {
                scattered = Ray(hr.p, reflected, rIn.time());
            } else {
                scattered = Ray(hr.p, refracted, rIn.time());
            }
        } else {
            scattered = Ray(hr.p, reflected, rIn.time());
        }
        return true;
    }
    case MatType::Isotropic: {
        scattered = Ray(hr.p, randomInSphere(states, tid), rIn.time());
        attenuation = sampleMaterialAlbedo(m, textures, hr.u, hr.v, hr.p);
        return true;
    }
    case MatType::Light:
    case MatType::DiffuseLight:
        return false;
    }
    return false;
}
