#pragma once

#include "cuda_annot.h"
#include "material.h"
#include "texture.h"

/**
 * \brief Emissive material.
 *
 * \p tex is non-owning (scene / device texture pool owns the texture).
 */
class DiffuseLight : public Material {
public:
    PATHTRACER_HD DiffuseLight(Texture* texture) : tex(texture) {}
    /** \brief Solid emit color; caller must track the allocated SolidColor. */
    PATHTRACER_HD DiffuseLight(const vec3& emit) : tex(new SolidColor(emit)) {}

    DiffuseLight(const DiffuseLight&) = delete;
    DiffuseLight& operator=(const DiffuseLight&) = delete;

    PATHTRACER_HD vec3 emit(float u, float v, const vec3& p) const override {
        return tex->value(u, v, p);
    }

    PATHTRACER_HD bool scatter(const Ray&, const HitRecord&, vec3&, Ray&) const override {
        return false;
    }

private:
    Texture* tex;
};
