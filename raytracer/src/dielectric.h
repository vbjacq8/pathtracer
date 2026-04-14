#pragma once

#include"hittables.h"
#include"my_random.h"


/**
 * \brief Shlick approximation for probability of 
 * reflection 
 * \param cosine cosine of the transmitted angle
 * \param nt transmission index of refraction
 */
float schlick(float cosine, float nt){
    float r0 = (1-nt) / (1 + nt);
    r0 = r0*r0;
    return r0 + (1-r0) * pow((1-cosine), 5);
}

class Dielectric : public Material {
    public:
        Dielectric(float ri) : nt(ri) {}
        virtual bool scatter(const Ray& rIn, const HitRecord& hr, vec3& attenuation, Ray& scattered) const override {
            vec3 outwardNormal;
            float R;
            vec3 v = rIn.direction();            
            vec3 refracted;
            attenuation = vec3(1.0,1.0,1.0);
            float cosine;
            //the below logic defines whether we are going from
            //higher to lower or lower to higher index of refraction.
            if (dot(v,hr.normal) < 0){
                R = 1/ nt;
                outwardNormal = hr.normal;
                cosine = -dot(v, hr.normal) / v.norm();

            }
            else {
                outwardNormal = -1 * hr.normal;
                cosine = nt * dot(v, hr.normal) / v.norm();

                R = nt;
                
            }
            vec3 reflected = reflect(v, outwardNormal);


            //the below logic defines the refracted ray behavior, if any
            if (refract(v, outwardNormal, R, refracted)){
                if (randomDouble(0, 1.0) < schlick(cosine, nt)){scattered = Ray(hr.p, reflected);}
                else{scattered = Ray(hr.p,refracted);}
            }
            else{
                scattered = Ray(hr.p,reflected);
            }
            return true;

        }


        vec3 reflect(const vec3& v, const vec3& n) const {
            return v - 2 * dot(v,n) * n;
        }


        /**
         * \brief output by reference refraction method that \returns true when
         * refraction happens, false when TIR occurs
         * \param v incident vector
         * \param n outgoing normal vector of object
         * \param R ratio ni/nt, where ni and nt are initial
         * and transmission indices of refraction, respectively
         * \param refracted refracted ray to be updated
         * 
         * 
         */
        bool refract(const vec3& v, const vec3& n, float R, vec3& refracted) const {
            vec3 uv = unit_vector(v);
            float dt = dot(uv,n);
            //This is cos^2 of the transmitted angle. If negative, it means
            //we are having an "internal reflection" because theta2 > pi/2
            float D = 1 - R*R * (1 - dt*dt);
            if (D > 0){
                refracted = R*(uv - n*dt) - n * sqrt(D);
                return true;
            }
            else {
                return false;}
            
        }

    vec3 attenuation;
    float nt;



};
