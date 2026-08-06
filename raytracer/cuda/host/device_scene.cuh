#pragma once

/**
 * Host-side CUDA scene builder — fully flat tables.
 *
 * Materials / textures / hitables are host vectors of POD records
 * (\p MaterialRec / \p TextureRec / \p HitableRec) uploaded once in
 * \p buildWorld(). No device \p new / virtuals.
 *
 * Teardown in DeviceScene::free(): cudaFree flat device tables.
 */

#include <cuda_runtime.h>

#include <utility>
#include <vector>

#include "check_cuda.cuh"

#include "../device/hitable_rec.cuh"
#include "../device/material_rec.cuh"

#include "../../src/vec3.h"

/**
 * \brief Host composition for one CUDA scene (flat tables only).
 *
 * \p add* appends records and returns table indices. Call \p free() once
 * (also from dtor).
 */
class DeviceScene {
public:
    DeviceScene() = default;
    ~DeviceScene() { free(); }

    DeviceScene(const DeviceScene&) = delete;
    DeviceScene& operator=(const DeviceScene&) = delete;

    DeviceScene(DeviceScene&& other) noexcept { moveFrom(std::move(other)); }

    DeviceScene& operator=(DeviceScene&& other) noexcept {
        if (this != &other) {
            free();
            moveFrom(std::move(other));
        }
        return *this;
    }

    // --- textures (flat TextureRec table; returns texIndex) ---

    int addSolidColor(vec3 c) {
        textureRecs_.push_back(makeSolidTextureRec(c));
        return static_cast<int>(textureRecs_.size()) - 1;
    }

    int addCheckered(float scale, vec3 even, vec3 odd) {
        textureRecs_.push_back(makeCheckeredTextureRec(scale, even, odd));
        return static_cast<int>(textureRecs_.size()) - 1;
    }

    int addNoise(float scale) {
        textureRecs_.push_back(makeNoiseTextureRec(scale));
        return static_cast<int>(textureRecs_.size()) - 1;
    }

    int addTexture(const TextureRec& rec) {
        textureRecs_.push_back(rec);
        return static_cast<int>(textureRecs_.size()) - 1;
    }

    // --- materials (flat MaterialRec table; returns matIndex) ---

    int addLambertian(int texIndex) {
        materialRecs_.push_back(makeLambertianRec(vec3(0.5f, 0.5f, 0.5f), texIndex));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addLambertian(vec3 albedo) {
        const int texIndex = addSolidColor(albedo);
        materialRecs_.push_back(makeLambertianRec(albedo, texIndex));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addMetal(vec3 albedo, float fuzz) {
        materialRecs_.push_back(makeMetalRec(albedo, fuzz));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addDielectric(float ir) {
        materialRecs_.push_back(makeDielectricRec(ir));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addLight(vec3 emission) {
        materialRecs_.push_back(makeLightRec(emission));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addDiffuseLight(int texIndex) {
        materialRecs_.push_back(makeDiffuseLightRec(vec3(1.0f, 1.0f, 1.0f), texIndex));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addDiffuseLight(vec3 emit) {
        const int texIndex = addSolidColor(emit);
        materialRecs_.push_back(makeDiffuseLightRec(emit, texIndex));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addIsotropic(int texIndex) {
        materialRecs_.push_back(makeIsotropicRec(vec3(0.5f, 0.5f, 0.5f), texIndex));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addIsotropic(vec3 albedo) {
        const int texIndex = addSolidColor(albedo);
        materialRecs_.push_back(makeIsotropicRec(albedo, texIndex));
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    int addMaterial(const MaterialRec& rec) {
        materialRecs_.push_back(rec);
        return static_cast<int>(materialRecs_.size()) - 1;
    }

    // --- hitables (flat HitableRec table; materials by matIndex) ---

    int addSphere(vec3 center, float radius, int matIndex) {
        hitableRecs_.push_back(makeSphereRec(center, radius, matIndex));
        return static_cast<int>(hitableRecs_.size()) - 1;
    }

    int addMovingSphere(vec3 c1, vec3 c2, float radius, int matIndex) {
        hitableRecs_.push_back(makeMovingSphereRec(c1, c2, radius, matIndex));
        return static_cast<int>(hitableRecs_.size()) - 1;
    }

    int addQuad(vec3 q, vec3 u, vec3 v, int matIndex) {
        hitableRecs_.push_back(makeQuadRec(q, u, v, matIndex));
        return static_cast<int>(hitableRecs_.size()) - 1;
    }

    int addHitable(const HitableRec& rec) {
        hitableRecs_.push_back(rec);
        return static_cast<int>(hitableRecs_.size()) - 1;
    }

    /**
     * \brief Uploads all flat tables to managed device memory.
     * \returns Device pointer to the hitable table (also \p hitables()).
     */
    const HitableRec* buildWorld() {
        uploadFlatTables();
        return dHitableRecs_;
    }

    const HitableRec* hitables() const { return dHitableRecs_; }
    int hitableCount() const { return static_cast<int>(hitableRecs_.size()); }

    /** Device pointers to flat tables (valid after \p buildWorld / \p uploadFlatTables). */
    const MaterialRec* materialRecs() const { return dMaterialRecs_; }
    const TextureRec* textureRecs() const { return dTextureRecs_; }
    int materialRecCount() const { return static_cast<int>(materialRecs_.size()); }
    int textureRecCount() const { return static_cast<int>(textureRecs_.size()); }

    void uploadFlatTables() {
        freeDeviceTables();

        const size_t nMat = materialRecs_.size();
        if (nMat > 0) {
            checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&dMaterialRecs_),
                                              nMat * sizeof(MaterialRec)));
            checkCudaErrors(cudaMemcpy(dMaterialRecs_, materialRecs_.data(),
                                       nMat * sizeof(MaterialRec), cudaMemcpyHostToDevice));
        }

        const size_t nTex = textureRecs_.size();
        if (nTex > 0) {
            checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&dTextureRecs_),
                                              nTex * sizeof(TextureRec)));
            checkCudaErrors(cudaMemcpy(dTextureRecs_, textureRecs_.data(),
                                       nTex * sizeof(TextureRec), cudaMemcpyHostToDevice));
        }

        const size_t nHit = hitableRecs_.size();
        if (nHit > 0) {
            checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&dHitableRecs_),
                                              nHit * sizeof(HitableRec)));
            checkCudaErrors(cudaMemcpy(dHitableRecs_, hitableRecs_.data(),
                                       nHit * sizeof(HitableRec), cudaMemcpyHostToDevice));
        }
    }

    /** Destroy flat device tables and clear host vectors. */
    void free() {
        freeDeviceTables();
        materialRecs_.clear();
        textureRecs_.clear();
        hitableRecs_.clear();
    }

private:
    void freeDeviceTables() {
        if (dMaterialRecs_ != nullptr) {
            checkCudaErrors(cudaFree(dMaterialRecs_));
            dMaterialRecs_ = nullptr;
        }
        if (dTextureRecs_ != nullptr) {
            checkCudaErrors(cudaFree(dTextureRecs_));
            dTextureRecs_ = nullptr;
        }
        if (dHitableRecs_ != nullptr) {
            checkCudaErrors(cudaFree(dHitableRecs_));
            dHitableRecs_ = nullptr;
        }
    }

    void moveFrom(DeviceScene&& other) {
        textureRecs_ = std::move(other.textureRecs_);
        materialRecs_ = std::move(other.materialRecs_);
        hitableRecs_ = std::move(other.hitableRecs_);
        dMaterialRecs_ = other.dMaterialRecs_;
        dTextureRecs_ = other.dTextureRecs_;
        dHitableRecs_ = other.dHitableRecs_;
        other.dMaterialRecs_ = nullptr;
        other.dTextureRecs_ = nullptr;
        other.dHitableRecs_ = nullptr;
    }

    std::vector<TextureRec> textureRecs_;
    std::vector<MaterialRec> materialRecs_;
    std::vector<HitableRec> hitableRecs_;
    MaterialRec* dMaterialRecs_ = nullptr;
    TextureRec* dTextureRecs_ = nullptr;
    HitableRec* dHitableRecs_ = nullptr;
};
