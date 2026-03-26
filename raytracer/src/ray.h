#ifndef RAY_H
#define RAY_H

#include"vec3.h"

class Ray{
    public:
        Ray(){};
        Ray(const vec3& a, const vec3& b){A = a; B = b;}


        /** \returns constant point A */
        vec3 origin() const {return A;}
        /** \returns constant velocity B */
        vec3 direction() const {return B;}
        /** \returns point after t time has elapsed \param time t */
        vec3 point_at_parameter(double t) const {return A + B * t;}

    private:
        vec3 A;
        vec3 B;


};

/** 
 * \brief paints a blue-white gradient by shooting rays from eye to xy-plane
 * \returns linear interpolation from blue to white */
vec3 colorBlueWhiteGradient(const Ray& r){
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);

}

double hitSphere(const vec3& center, double radius, const Ray& r){
    vec3 oc = r.origin() - center;
    double b = 2.0 * dot(oc,r.direction());
    double a = dot(r.direction(), r.direction());
    double c = dot(oc, oc) - radius*radius;
    double discriminant = b*b - 4*a*c;
    if (discriminant < 0){
        return -1.0;
    }
    else{
        return (-b - sqrt(discriminant)) / (2 * a);
    }

}


vec3 colorSphere(const Ray& r, const vec3& center, double radius, const vec3& color){
    if (hitSphere(center, radius, r) > 0.0){
        return color;
    }
    //vec3 unit_direction = unit_vector(r.direction());
    //double t = 0.5*(unit_direction.y() + 1.0);
    //return (1-t) * vec3(1.0,1.0,1.0) + t * vec3(0.5, 0.7, 1.0);
    return colorBlueWhiteGradient(r);
}

//TODO: implement color with shading
vec3 colorShadedSphere(const Ray& r, const vec3& center, double radius, const vec3& color){
    double t = hitSphere(center, radius, r);
   if (t > 0.0){
        vec3 N = r.point_at_parameter(t) - center;
        vec3 n = unit_vector(N * color);
        return vec3(n.x()+1, n.y()+1,n.z()+1) * 0.5;
    }
    return colorBlueWhiteGradient(r);

}

vec3 colorShadedSphere(const Ray& r, const vec3& center, double radius){
    double t = hitSphere(center, radius, r);
    if (t > 0.0){
        vec3 N = unit_vector(r.point_at_parameter(t) - center);
        return vec3(N.x()+1, N.y()+1,N.z()+1) * 0.5;
    }
    return colorBlueWhiteGradient(r);

}



#endif
