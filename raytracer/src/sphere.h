#ifndef SPHERE_H
#define SPHERE_H

#include"hitable.h"

class Sphere : public Hitable{
    public:
        Sphere() {};
        Sphere(const vec3& cen, double R) : center(cen), radius(R) {};
        virtual bool hit(const Ray& r, double tMin, double tMax, HitRecord& hr);


    vec3 center;
    double radius;

};

/** \brief hit implementation for sphere
 *  \copydoc Hitable::hit
 */
bool Sphere::hit(const Ray& r, double tMin, double tMax, HitRecord& hr){
    vec3 oc = r.origin() - center;
    double b = 2.0 * dot(oc,r.direction());
    double a = dot(r.direction(), r.direction());
    double c = dot(oc, oc) - radius*radius;
    double discriminant = b*b - 4*a*c;
    if (discriminant < 0){
        return false;
    }
    double temp = (-b - sqrt(discriminant))/(2.0 * a);
    if (temp > tMin && temp < tMax){
        hr.t = temp;
        hr.p = r.point_at_parameter(temp);
        hr.normal = (r.point_at_parameter(temp) - center)/radius;
        return true;}
    temp = (-b + sqrt(discriminant))/(2.0*a);
    if (temp > tMin && temp < tMax){
        hr.t = temp;
        hr.p = r.point_at_parameter(temp);
        hr.normal = (r.point_at_parameter(temp) - center)/radius;
        return true;}
    return false;
}



#endif
