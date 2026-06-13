#pragma once
#include"hitable_list.h"
#include<float.h>
#include"my_random.h"

/**
 * \brief generic coloring method with blueWhiteGradient background
 * \returns a color vector, based on the normal of 
 * \param r Ray object that is sent to the viewport and examined for hits
 * \param world Hitable array that is to be colored.
 */
inline vec3 color(const Ray& r, Hitable* world, int depth, int i = 0){
    (void) depth;
    (void) i;
    HitRecord hr;
    if (world->hit(r, 0.001, MAXFLOAT, hr)){
        return 0.5 * vec3(hr.normal.x()+1.0, hr.normal.y()+1.0, hr.normal.z()+1.0);
    }
    return colorBlueWhiteGradient(r);
}

/**
 * \brief matte coloring method with blueWhiteGradient background
 * \returns a color vector, based on ideal diffuse material (random reflection)
 * \param r Ray object that is sent to the viewport and examined for hits
 * \param world Hitable array that is to be colored.
 */
inline vec3 diffuseColor(const Ray& r, Hitable* world, int depth, int i = 0){
    if (i >= depth){return vec3(0,0,0);}
    HitRecord hr;
    if (world->hit(r,0.001, MAXFLOAT, hr)){
        Ray scattered;
        vec3 attenuation;
        if (hr.matPtr && hr.matPtr->scatter(r, hr, attenuation, scattered)){
            return attenuation * diffuseColor(scattered, world, depth, i +1);
        }
        else{throw std::runtime_error("scatter returned false");}
    }
    else{return colorBlueWhiteGradient(r);}
}

/**
 * \brief metal coloring method with blueWhiteGradient background
 * \returns a color vector, based on reflective metal material (deterministic reflection)
 * \param r Ray object that is sent to the viewport and examined for hits
 * \param world Hitable array that is to be colored.
 */
inline vec3 metalColor(const Ray& r, Hitable* world, int depth, int i = 0){
    if (i >= depth){return vec3(0,0,0);}
    HitRecord hr;
    if (world->hit(r,0.001, MAXFLOAT, hr)){
        Ray scattered;
        vec3 attenuation;
        if (hr.matPtr && hr.matPtr->scatter(r, hr, attenuation, scattered)){
            return attenuation * metalColor(scattered, world, depth, i + 1);
        }
        else{
            if (!hr.matPtr){throw std::runtime_error("null matPtr");}
            //scatter returned false; absorbed
            return vec3(0,0,0);
        }
    }
    else {return colorBlueWhiteGradient(r);}

}

inline vec3 dielectricColor(const Ray& r, Hitable* world, int depth, int i = 0){
    if (i >= depth){return vec3(0,0,0);}
    HitRecord hr;
    if (world->hit(r,0.005, MAXFLOAT, hr)){
        Ray scattered;
        vec3 attenuation;
        if (hr.matPtr && hr.matPtr->scatter(r, hr, attenuation, scattered)){
            return attenuation * dielectricColor(scattered, world, depth, i+1);
        }
        else{
            if (!hr.matPtr){throw std::runtime_error("null matPtr");}
            //scatter returned false; absorbed
            return vec3(0,0,0);
        }
    }
    else {return colorBlueWhiteGradient(r);}
}

