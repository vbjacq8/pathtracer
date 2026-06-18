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

    Hitable** list;
    int listSize;
};

#endif
