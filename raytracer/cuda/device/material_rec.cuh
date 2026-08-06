#pragma once

#include "../../src/ray.h"
#include "../../src/vec3.h"

/**
 * Flat device material / texture records for CUDA path tracing.
 * No virtuals — dispatch via MatType / TexType switches.
 */

enum class MatType : int {
    Lambertian = 0,
    Metal,
    Dielectric,
    Light,
    DiffuseLight,
    Isotropic
};

enum class TexType : int {
    Solid = 0,
    Checkered,
    Noise
};

struct TextureRec {
    TexType type = TexType::Solid;
    vec3 color0{0.0f, 0.0f, 0.0f}; ///< solid albedo, or checkered "even"
    vec3 color1{0.0f, 0.0f, 0.0f}; ///< checkered "odd"
    float invScale = 1.0f;         ///< 1 / checker scale
    float scale = 1.0f;            ///< noise spatial scale (CPU Noise::scale)
};

/**
 * \brief One material entry in a device material table.
 *
 * - Lambertian / Isotropic / DiffuseLight: sample \p texIndex, or \p albedo if texIndex < 0
 * - Metal: \p albedo + \p fuzz
 * - Dielectric: \p ior (\p nt)
 * - Light: emission stored in \p albedo
 */
struct MaterialRec {
    MatType type = MatType::Lambertian;
    vec3 albedo{0.5f, 0.5f, 0.5f};
    float fuzz = 0.0f;
    float ior = 1.0f;
    int texIndex = -1; ///< index into TextureRec table; -1 → use albedo
};

/** \brief Host/device helpers to build table entries. */
inline MaterialRec makeLambertianRec(const vec3& albedo, int texIndex = -1) {
    MaterialRec m;
    m.type = MatType::Lambertian;
    m.albedo = albedo;
    m.texIndex = texIndex;
    return m;
}

inline MaterialRec makeMetalRec(const vec3& albedo, float fuzz) {
    MaterialRec m;
    m.type = MatType::Metal;
    m.albedo = albedo;
    m.fuzz = fuzz < 0.0f ? 0.0f : (fuzz > 1.0f ? 1.0f : fuzz);
    return m;
}

inline MaterialRec makeDielectricRec(float ior) {
    MaterialRec m;
    m.type = MatType::Dielectric;
    m.ior = ior;
    m.albedo = vec3(1.0f, 1.0f, 1.0f);
    return m;
}

inline MaterialRec makeLightRec(const vec3& emission) {
    MaterialRec m;
    m.type = MatType::Light;
    m.albedo = emission;
    return m;
}

inline MaterialRec makeDiffuseLightRec(const vec3& emission, int texIndex = -1) {
    MaterialRec m;
    m.type = MatType::DiffuseLight;
    m.albedo = emission;
    m.texIndex = texIndex;
    return m;
}

inline MaterialRec makeIsotropicRec(const vec3& albedo, int texIndex = -1) {
    MaterialRec m;
    m.type = MatType::Isotropic;
    m.albedo = albedo;
    m.texIndex = texIndex;
    return m;
}

inline TextureRec makeSolidTextureRec(const vec3& albedo) {
    TextureRec t;
    t.type = TexType::Solid;
    t.color0 = albedo;
    return t;
}

inline TextureRec makeCheckeredTextureRec(float scale, const vec3& even, const vec3& odd) {
    TextureRec t;
    t.type = TexType::Checkered;
    t.color0 = even;
    t.color1 = odd;
    t.invScale = 1.0f / scale;
    return t;
}

inline TextureRec makeNoiseTextureRec(float scale) {
    TextureRec t;
    t.type = TexType::Noise;
    t.scale = scale;
    return t;
}

/** \brief Cheap hash in [0, 1) for device noise (no Perlin tables). */
__device__ inline float hash31(float x, float y, float z) {
    float n = sinf(x * 12.9898f + y * 78.233f + z * 37.719f) * 43758.5453f;
    return n - floorf(n);
}

__device__ inline float valueNoise(const vec3& p) {
    const float ix = floorf(p.x());
    const float iy = floorf(p.y());
    const float iz = floorf(p.z());
    const float fx = p.x() - ix;
    const float fy = p.y() - iy;
    const float fz = p.z() - iz;
    const float ux = fx * fx * (3.0f - 2.0f * fx);
    const float uy = fy * fy * (3.0f - 2.0f * fy);
    const float uz = fz * fz * (3.0f - 2.0f * fz);

    const float n000 = hash31(ix, iy, iz);
    const float n100 = hash31(ix + 1.0f, iy, iz);
    const float n010 = hash31(ix, iy + 1.0f, iz);
    const float n110 = hash31(ix + 1.0f, iy + 1.0f, iz);
    const float n001 = hash31(ix, iy, iz + 1.0f);
    const float n101 = hash31(ix + 1.0f, iy, iz + 1.0f);
    const float n011 = hash31(ix, iy + 1.0f, iz + 1.0f);
    const float n111 = hash31(ix + 1.0f, iy + 1.0f, iz + 1.0f);

    const float nx00 = n000 * (1.0f - ux) + n100 * ux;
    const float nx10 = n010 * (1.0f - ux) + n110 * ux;
    const float nx01 = n001 * (1.0f - ux) + n101 * ux;
    const float nx11 = n011 * (1.0f - ux) + n111 * ux;
    const float nxy0 = nx00 * (1.0f - uy) + nx10 * uy;
    const float nxy1 = nx01 * (1.0f - uy) + nx11 * uy;
    return nxy0 * (1.0f - uz) + nxy1 * uz;
}

/** \brief Turbulence matching CPU Perlin::turb depth loop (hash-based). */
__device__ inline float hashTurb(const vec3& p, int depth) {
    float accum = 0.0f;
    float weight = 1.0f;
    vec3 temp = p;
    for (int i = 0; i < depth; ++i) {
        accum += weight * valueNoise(temp);
        weight *= 0.5f;
        temp *= 2.0f;
    }
    return fabsf(accum);
}

/**
 * \brief Sample a texture table entry (or fall back to \p fallback).
 */
__device__ inline vec3 sampleTexture(const TextureRec* textures, int texIndex, const vec3& fallback,
                                     float /*u*/, float /*v*/, const vec3& p) {
    if (texIndex < 0 || textures == nullptr) {
        return fallback;
    }
    const TextureRec& tex = textures[texIndex];
    switch (tex.type) {
    case TexType::Solid:
        return tex.color0;
    case TexType::Checkered: {
        const int x = static_cast<int>(floorf(tex.invScale * p.x()));
        const int y = static_cast<int>(floorf(tex.invScale * p.y()));
        const int z = static_cast<int>(floorf(tex.invScale * p.z()));
        return ((x + y + z) & 1) == 0 ? tex.color0 : tex.color1;
    }
    case TexType::Noise: {
        // Same marble formula as CPU Noise::value; turb is hash-based (no Perlin tables).
        const float s =
            sinf(tex.scale * p.z() * p.x() + 10.0f * hashTurb(p, 7));
        return vec3(0.5f, 0.5f, 0.5f) * (1.0f + s);
    }
    }
    return fallback;
}

__device__ inline vec3 sampleMaterialAlbedo(const MaterialRec& m, const TextureRec* textures,
                                            float u, float v, const vec3& p) {
    return sampleTexture(textures, m.texIndex, m.albedo, u, v, p);
}

/**
 * \brief Emitted radiance for lights; zero for ordinary materials.
 */
__device__ inline vec3 emitMaterial(const MaterialRec& m, const TextureRec* textures, float u,
                                    float v, const vec3& p) {
    switch (m.type) {
    case MatType::Light:
        return m.albedo;
    case MatType::DiffuseLight:
        return sampleMaterialAlbedo(m, textures, u, v, p);
    default:
        return vec3(0.0f, 0.0f, 0.0f);
    }
}
