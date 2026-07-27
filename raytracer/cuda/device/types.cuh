#pragma once

#include "ray.cuh"
#include "vec3.cuh"

/** Flat material kinds — replace CPU virtual Material hierarchy on device. */
enum class MaterialType : int {
    Lambertian = 0,
    Metal = 1,
    Dielectric = 2,
    DiffuseLight = 3,
};

struct MaterialDesc {
    MaterialType type;
    Vec3 albedo;
    float fuzz;       ///< metal roughness [0, 1]
    float ior;        ///< dielectric index of refraction
    Vec3 emit;        ///< diffuse light emission
};

/** Placeholder hit record; wire to flattened scene buffers next. */
struct HitRecord {
    float t;
    Vec3 p;
    Vec3 normal;
    float u, v;
    bool frontFace;
    int materialIndex;
};

/** Render launch parameters shared by host and device. */
struct RenderParams {
    int width;
    int height;
    int samplesPerPixel;
    int maxDepth;
};
