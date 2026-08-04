#pragma once

/**
 * Host-side CUDA scene builder: device factories + material/texture/hitable pools.
 *
 * Polymorphic objects are constructed on the device (correct vtables). The host
 * only stores device pointers and composes the scene — same workflow as CPU
 * per-file scenes, without a bespoke createWorld/freeWorld per demo.
 *
 * Teardown order in DeviceScene::free():
 *   world wrapper → hitables → materials → textures → cudaFree(dList)
 */

#include <cuda_runtime.h>

#include <utility>
#include <vector>

#include "check_cuda.cuh"

#include "../../src/hittables.h"
#include "../../src/materials.h"

// ---------------------------------------------------------------------------
// Device factories / deleters (one thread)
// ---------------------------------------------------------------------------

namespace cuda_factory {

__global__ void makeSolidColor(Texture** out, vec3 c) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new SolidColor(c);
    }
}

__global__ void makeLambertian(Material** out, Texture* tex) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Lambertian(tex);
    }
}

__global__ void makeMetal(Material** out, vec3 albedo, float fuzz) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Metal(albedo, fuzz);
    }
}

__global__ void makeDielectric(Material** out, float ir) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Dielectric(ir);
    }
}

__global__ void makeLight(Material** out, vec3 emission) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Light(emission);
    }
}

__global__ void makeDiffuseLight(Material** out, Texture* tex) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new DiffuseLight(tex);
    }
}

__global__ void makeIsotropic(Material** out, Texture* tex) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Isotropic(tex);
    }
}

__global__ void makeSphere(Hitable** out, vec3 center, float radius, Material* mat) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Sphere(center, radius, mat);
    }
}

__global__ void makeMovingSphere(Hitable** out, vec3 c1, vec3 c2, float radius, Material* mat) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Sphere(c1, c2, radius, mat);
    }
}

__global__ void makeQuad(Hitable** out, vec3 q, vec3 u, vec3 v, Material* mat) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new Quad(q, u, v, mat);
    }
}

__global__ void makeHitableList(Hitable** out, Hitable** list, int n) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        *out = new HitableList(list, n);  // non-owning view of list
    }
}

__global__ void destroyTexture(Texture* p) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        delete p;
    }
}

__global__ void destroyMaterial(Material* p) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        delete p;
    }
}

__global__ void destroyHitable(Hitable* p) {
    if (threadIdx.x == 0 && blockIdx.x == 0) {
        delete p;
    }
}

}  // namespace cuda_factory

// ---------------------------------------------------------------------------
// Host wrappers: launch factory, return device pointer
// ---------------------------------------------------------------------------

inline Texture* cudaNewSolidColor(vec3 c) {
    Texture** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Texture*)));
    *slot = nullptr;
    cuda_factory::makeSolidColor<<<1, 1>>>(slot, c);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Texture* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Material* cudaNewLambertian(Texture* tex) {
    Material** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Material*)));
    *slot = nullptr;
    cuda_factory::makeLambertian<<<1, 1>>>(slot, tex);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Material* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Material* cudaNewMetal(vec3 albedo, float fuzz) {
    Material** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Material*)));
    *slot = nullptr;
    cuda_factory::makeMetal<<<1, 1>>>(slot, albedo, fuzz);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Material* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Material* cudaNewDielectric(float ir) {
    Material** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Material*)));
    *slot = nullptr;
    cuda_factory::makeDielectric<<<1, 1>>>(slot, ir);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Material* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Material* cudaNewLight(vec3 emission) {
    Material** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Material*)));
    *slot = nullptr;
    cuda_factory::makeLight<<<1, 1>>>(slot, emission);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Material* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Material* cudaNewDiffuseLight(Texture* tex) {
    Material** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Material*)));
    *slot = nullptr;
    cuda_factory::makeDiffuseLight<<<1, 1>>>(slot, tex);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Material* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Material* cudaNewIsotropic(Texture* tex) {
    Material** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Material*)));
    *slot = nullptr;
    cuda_factory::makeIsotropic<<<1, 1>>>(slot, tex);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Material* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Hitable* cudaNewSphere(vec3 center, float radius, Material* mat) {
    Hitable** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Hitable*)));
    *slot = nullptr;
    cuda_factory::makeSphere<<<1, 1>>>(slot, center, radius, mat);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Hitable* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Hitable* cudaNewMovingSphere(vec3 c1, vec3 c2, float radius, Material* mat) {
    Hitable** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Hitable*)));
    *slot = nullptr;
    cuda_factory::makeMovingSphere<<<1, 1>>>(slot, c1, c2, radius, mat);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Hitable* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Hitable* cudaNewQuad(vec3 q, vec3 u, vec3 v, Material* mat) {
    Hitable** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Hitable*)));
    *slot = nullptr;
    cuda_factory::makeQuad<<<1, 1>>>(slot, q, u, v, mat);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Hitable* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline Hitable* cudaNewHitableList(Hitable** dList, int n) {
    Hitable** slot = nullptr;
    checkCudaErrors(cudaMallocManaged(reinterpret_cast<void**>(&slot), sizeof(Hitable*)));
    *slot = nullptr;
    cuda_factory::makeHitableList<<<1, 1>>>(slot, dList, n);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
    Hitable* ptr = *slot;
    checkCudaErrors(cudaFree(slot));
    return ptr;
}

inline void cudaDeleteTexture(Texture* p) {
    if (!p) {
        return;
    }
    cuda_factory::destroyTexture<<<1, 1>>>(p);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
}

inline void cudaDeleteMaterial(Material* p) {
    if (!p) {
        return;
    }
    cuda_factory::destroyMaterial<<<1, 1>>>(p);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
}

inline void cudaDeleteHitable(Hitable* p) {
    if (!p) {
        return;
    }
    cuda_factory::destroyHitable<<<1, 1>>>(p);
    checkCudaErrors(cudaGetLastError());
    checkCudaErrors(cudaDeviceSynchronize());
}

// ---------------------------------------------------------------------------
// DeviceScene — host composition + pools
// ---------------------------------------------------------------------------

/**
 * \brief Tracks device allocations for one CUDA scene.
 *
 * Host wrappers (\p cudaNew*) launch factories and return device pointers; this
 * class records them in pools. Hitables borrow materials; materials borrow
 * textures. Call \p free() once (also invoked from the destructor).
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

    // --- textures (pool-owned) ---

    Texture* addSolidColor(vec3 c) {
        Texture* t = cudaNewSolidColor(c);
        textures_.push_back(t);
        return t;
    }

    /** Register an already device-new'd texture (e.g. from a custom factory). */
    Texture* addTexture(Texture* t) {
        textures_.push_back(t);
        return t;
    }

    // --- materials (pool-owned; textures must already be in the texture pool) ---

    Material* addLambertian(Texture* tex) {
        Material* m = cudaNewLambertian(tex);
        materials_.push_back(m);
        return m;
    }

    Material* addLambertian(vec3 albedo) { return addLambertian(addSolidColor(albedo)); }

    Material* addMetal(vec3 albedo, float fuzz) {
        Material* m = cudaNewMetal(albedo, fuzz);
        materials_.push_back(m);
        return m;
    }

    Material* addDielectric(float ir) {
        Material* m = cudaNewDielectric(ir);
        materials_.push_back(m);
        return m;
    }

    Material* addLight(vec3 emission) {
        Material* m = cudaNewLight(emission);
        materials_.push_back(m);
        return m;
    }

    Material* addDiffuseLight(Texture* tex) {
        Material* m = cudaNewDiffuseLight(tex);
        materials_.push_back(m);
        return m;
    }

    Material* addDiffuseLight(vec3 emit) { return addDiffuseLight(addSolidColor(emit)); }

    Material* addIsotropic(Texture* tex) {
        Material* m = cudaNewIsotropic(tex);
        materials_.push_back(m);
        return m;
    }

    Material* addIsotropic(vec3 albedo) { return addIsotropic(addSolidColor(albedo)); }

    Material* addMaterial(Material* m) {
        materials_.push_back(m);
        return m;
    }

    // --- hitables (pool-owned; materials must already be in the material pool) ---

    Hitable* addSphere(vec3 center, float radius, Material* mat) {
        Hitable* h = cudaNewSphere(center, radius, mat);
        hitables_.push_back(h);
        return h;
    }

    Hitable* addMovingSphere(vec3 c1, vec3 c2, float radius, Material* mat) {
        Hitable* h = cudaNewMovingSphere(c1, c2, radius, mat);
        hitables_.push_back(h);
        return h;
    }

    Hitable* addQuad(vec3 q, vec3 u, vec3 v, Material* mat) {
        Hitable* h = cudaNewQuad(q, u, v, mat);
        hitables_.push_back(h);
        return h;
    }

    Hitable* addHitable(Hitable* h) {
        hitables_.push_back(h);
        return h;
    }

    /**
     * \brief Uploads hitable pointers to device and wraps them in a HitableList.
     * \returns Device pointer to the list (also stored as \p world()).
     */
    Hitable* buildWorld() {
        const int n = static_cast<int>(hitables_.size());
        if (dList_ != nullptr) {
            checkCudaErrors(cudaFree(dList_));
            dList_ = nullptr;
        }
        if (dWorld_ != nullptr) {
            cudaDeleteHitable(dWorld_);
            dWorld_ = nullptr;
        }

        checkCudaErrors(cudaMalloc(reinterpret_cast<void**>(&dList_), size_t(n) * sizeof(Hitable*)));
        if (n > 0) {
            checkCudaErrors(cudaMemcpy(dList_, hitables_.data(), size_t(n) * sizeof(Hitable*),
                                       cudaMemcpyHostToDevice));
        }
        dWorld_ = cudaNewHitableList(dList_, n);
        return dWorld_;
    }

    Hitable* world() const { return dWorld_; }
    Hitable** objectList() const { return dList_; }
    int objectCount() const { return static_cast<int>(hitables_.size()); }

    /** Destroy world → hitables → materials → textures (generic for every scene). */
    void free() {
        if (dWorld_ != nullptr) {
            cudaDeleteHitable(dWorld_);
            dWorld_ = nullptr;
        }
        for (Hitable* h : hitables_) {
            cudaDeleteHitable(h);
        }
        hitables_.clear();

        if (dList_ != nullptr) {
            checkCudaErrors(cudaFree(dList_));
            dList_ = nullptr;
        }

        for (Material* m : materials_) {
            cudaDeleteMaterial(m);
        }
        materials_.clear();

        for (Texture* t : textures_) {
            cudaDeleteTexture(t);
        }
        textures_.clear();
    }

private:
    void moveFrom(DeviceScene&& other) {
        textures_ = std::move(other.textures_);
        materials_ = std::move(other.materials_);
        hitables_ = std::move(other.hitables_);
        dList_ = other.dList_;
        dWorld_ = other.dWorld_;
        other.dList_ = nullptr;
        other.dWorld_ = nullptr;
    }

    std::vector<Texture*> textures_;
    std::vector<Material*> materials_;
    std::vector<Hitable*> hitables_;
    Hitable** dList_ = nullptr;
    Hitable* dWorld_ = nullptr;
};
