#ifndef RAY_H
#define RAY_H

#include"vec3.h"

class Ray{
    public:
        Ray(){};
        Ray(const vec3& a, const vec3& b){A = a; B = b;}


        /** \returns constnat point A */
        vec3 origin() const {return A;}
        /** \returns constant velocity B */
        vec3 direction() const {return B;}
        /** \returns point after t time has elapsed \param time t */
        vec3 point_at_parameter(double t) const {return A + B * t;}

    private:
        vec3 A;
        vec3 B;


};

vec3 color(const Ray& r){
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);

}

#endif