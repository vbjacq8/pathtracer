#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include"sphere.h"
#include<iostream>

class HitableList : public Hitable{
    public:
        HitableList(){};
        HitableList(Hitable** l, int n){
            list = l;
            listSize = n;
        }


 /** \brief iterates through the whole hit checklist and updates the HitRecord instance
  *  with the hit that has the lowest t
  *  \param r ray to check
  *  \param tMin min accepted t that counts as a hit
  *  \param tMax max accepted t that counts as a hit
  *  \param hr hit record that may update
  *  \returns boolean for whether ray hit something
 */
        bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) override {
            HitRecord tempRec;
            bool hitAnything = false;
            double closestSoFar = tMax;

            for (int i = 0; i < listSize; i++){
                if (list[i]->hit(r, tMin, closestSoFar, tempRec)){
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
