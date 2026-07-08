#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include "sphere.h"
#include <iostream>

/**
 * \brief Scene container that returns the closest hit among its children.
 */
class HitableList : public Hitable {
public:
    HitableList() {}
    HitableList(Hitable** l, int n) {
        list = l;
        listSize = n;
    }

    /**
     * \brief Finds the nearest hit among all children.
     * \copydoc Hitable::hit
     */
    bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override {
        HitRecord tempRec;
        bool hitAnything = false;
        double closestSoFar = tMax;

        for (int i = 0; i < listSize; i++) {
            if (list[i]->hit(r, tMin, closestSoFar, tempRec)) {
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
        for (int i = 0; i < listSize; i++) {
            AABB childBox = list[i]->boundingBox();
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
        if (listSize == 0) {
            return sum;
        }
        for (int i = 0; i < listSize; i++) {
            sum += list[i]->centroid();
        }
        return sum / listSize;
    }

    Hitable** list;
    int listSize;
};

#endif
