#ifndef HITABLE_H
#define HITABLE_H

#include"ray.h"

class Material;

struct HitRecord{
    double t;
    vec3 p;
    vec3 normal;
    Material* matPtr;

};

class Hitable{
    public:
        /** \param r ray to check
         *  \param tMin min accepted t that counts as a hit
         *  \param tMax max accepted t that counts as sa hit
         *  \param hr hit record that may update
         *  \returns boolean for whether ray hit something
         */
        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr) = 0;

};



#endif
