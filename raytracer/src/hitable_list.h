#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include "hitable.h"

#include <memory>
#include <vector>

/**
 * \brief Scene container that returns the closest hit among its children.
 */
class HitableList : public Hitable {
public:
    HitableList() = default;
    explicit HitableList(const std::vector<HitablePtr>& objects) : objects_(objects) {}
    explicit HitableList(std::vector<HitablePtr>&& objects) : objects_(std::move(objects)) {}

    void clear() { objects_.clear(); }

    void add(HitablePtr object) { objects_.push_back(std::move(object)); }

    /**
     * \brief Finds the nearest hit among all children.
     * \copydoc Hitable::hit
     */
    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override {
        HitRecord tempRec;
        bool hitAnything = false;
        double closestSoFar = tMax;

        for (const auto& object : objects_) {
            if (object->hit(r, tMin, closestSoFar, tempRec)) {
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
    AABB boundingBox() const override {
        AABB box;
        box.min = vec3(1e30, 1e30, 1e30);
        box.max = vec3(-1e30, -1e30, -1e30);
        for (const auto& object : objects_) {
            AABB childBox = object->boundingBox();
            box.min = min3(box.min, childBox.min);
            box.max = max3(box.max, childBox.max);
        }
        return box;
    }

    /**
     * \returns Average of child centroids.
     */
    vec3 centroid() const override {
        vec3 sum(0, 0, 0);
        if (objects_.empty()) {
            return sum;
        }
        for (const auto& object : objects_) {
            sum += object->centroid();
        }
        return sum / static_cast<double>(objects_.size());
    }

    std::vector<HitablePtr>& objects() { return objects_; }
    const std::vector<HitablePtr>& objects() const { return objects_; }

private:
    std::vector<HitablePtr> objects_;
};

#endif
