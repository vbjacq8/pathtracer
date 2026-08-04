#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include "constants.h"
#include "cuda_annot.h"
#include "hitable.h"

#include <memory>
#include <utility>
#include <vector>

/**
 * \brief Scene container that returns the closest hit among its children.
 *
 * Children are stored in a flat \p Hitable* array (GPU-indexable). Host
 * \p add(HitablePtr) retains ownership in a heap \p owned_ vector. A non-owning
 * view ctor wraps an existing array (e.g. device pointer + count) without
 * deleting it — safe to \p new on device because no \p std::vector member is
 * embedded in the object.
 */
class HitableList : public Hitable {
public:
    PATHTRACER_HD HitableList()
        : list_(nullptr), listSize_(0), listCapacity_(0), ownsList_(true), owned_(nullptr) {}

    /** Non-owning view of an existing flat array (host or device). */
    PATHTRACER_HD HitableList(Hitable** list, int size)
        : list_(list), listSize_(size), listCapacity_(size), ownsList_(false), owned_(nullptr) {}

    explicit HitableList(const std::vector<HitablePtr>& objects)
        : list_(nullptr), listSize_(0), listCapacity_(0), ownsList_(true), owned_(nullptr) {
        reserve(static_cast<int>(objects.size()));
        for (const auto& object : objects) {
            add(object);
        }
    }

    explicit HitableList(std::vector<HitablePtr>&& objects)
        : list_(nullptr), listSize_(0), listCapacity_(0), ownsList_(true), owned_(nullptr) {
        reserve(static_cast<int>(objects.size()));
        for (auto& object : objects) {
            add(std::move(object));
        }
    }

    PATHTRACER_HD ~HitableList() override {
        freeList();
#if !defined(__CUDA_ARCH__)
        delete owned_;
        owned_ = nullptr;
#endif
    }

    HitableList(const HitableList&) = delete;
    HitableList& operator=(const HitableList&) = delete;

    HitableList(HitableList&& other) noexcept
        : list_(other.list_)
        , listSize_(other.listSize_)
        , listCapacity_(other.listCapacity_)
        , ownsList_(other.ownsList_)
        , owned_(other.owned_) {
        other.list_ = nullptr;
        other.listSize_ = 0;
        other.listCapacity_ = 0;
        other.ownsList_ = true;
        other.owned_ = nullptr;
    }

    HitableList& operator=(HitableList&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        freeList();
#if !defined(__CUDA_ARCH__)
        delete owned_;
#endif
        list_ = other.list_;
        listSize_ = other.listSize_;
        listCapacity_ = other.listCapacity_;
        ownsList_ = other.ownsList_;
        owned_ = other.owned_;
        other.list_ = nullptr;
        other.listSize_ = 0;
        other.listCapacity_ = 0;
        other.ownsList_ = true;
        other.owned_ = nullptr;
        return *this;
    }

    void clear() {
        listSize_ = 0;
        if (owned_ != nullptr) {
            owned_->clear();
        }
    }

    /** Append a non-owning pointer (grows the flat array on the host). */
    void add(Hitable* object) {
        if (listSize_ >= listCapacity_) {
            reserve(listCapacity_ == 0 ? 8 : listCapacity_ * 2);
        }
        list_[listSize_++] = object;
    }

    /** Append and retain ownership for the list lifetime. */
    void add(HitablePtr object) {
        if (owned_ == nullptr) {
            owned_ = new std::vector<HitablePtr>();
        }
        owned_->push_back(std::move(object));
        add(owned_->back().get());
    }

    /**
     * \brief Finds the nearest hit among all children.
     * \copydoc Hitable::hit
     */
    PATHTRACER_HD bool hit(const Ray& r, float tMin, float tMax, HitRecord& hr) override {
        HitRecord tempRec;
        bool hitAnything = false;
        float closestSoFar = tMax;

        for (int i = 0; i < listSize_; ++i) {
            if (list_[i]->hit(r, tMin, closestSoFar, tempRec)) {
                hitAnything = true;
                closestSoFar = tempRec.t;
                hr = tempRec;
            }
        }
        return hitAnything;
    }

    /**
     * \returns Union of all child bounding boxes.
     */
    PATHTRACER_HD AABB boundingBox() const override {
        AABB box;
        box.min = vec3(+infinity, +infinity, +infinity);
        box.max = vec3(-infinity, -infinity, -infinity);
        for (int i = 0; i < listSize_; ++i) {
            AABB childBox = list_[i]->boundingBox();
            box.min = min3(box.min, childBox.min);
            box.max = max3(box.max, childBox.max);
        }
        return box;
    }

    /**
     * \returns Average of child centroids (better for BVH than the union-AABB center).
     */
    PATHTRACER_HD vec3 centroid() const override {
        vec3 sum(0, 0, 0);
        if (listSize_ == 0) {
            return sum;
        }
        for (int i = 0; i < listSize_; ++i) {
            sum += list_[i]->centroid();
        }
        return sum / static_cast<float>(listSize_);
    }

    PATHTRACER_HD Hitable** objects() { return list_; }
    PATHTRACER_HD Hitable** objects() const { return list_; }
    PATHTRACER_HD int size() const { return listSize_; }

private:
    PATHTRACER_HD void freeList() {
        if (ownsList_ && list_ != nullptr) {
            delete[] list_;
        }
        list_ = nullptr;
        listSize_ = 0;
        listCapacity_ = 0;
    }

    void reserve(int capacity) {
        if (capacity <= listCapacity_) {
            return;
        }
        Hitable** neu = new Hitable*[static_cast<size_t>(capacity)];
        for (int i = 0; i < listSize_; ++i) {
            neu[i] = list_[i];
        }
        if (ownsList_ && list_ != nullptr) {
            delete[] list_;
        }
        list_ = neu;
        listCapacity_ = capacity;
        ownsList_ = true;
    }

    Hitable** list_;
    int listSize_;
    int listCapacity_;
    bool ownsList_;
    /** Host-only ownership bag; always null for device view lists. */
    std::vector<HitablePtr>* owned_;
};

#endif
