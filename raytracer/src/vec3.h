#ifndef VEC3_H
#define VEC3_H

#include<math.h>
#include<iostream>
#include<stdlib.h>

class vec3{
    public:
        vec3(){}
        vec3(float e1, float e2, float e3){}
        inline float x() const;
        inline float y() const;
        inline float z() const;
        inline float r() const;
        inline float g() const;
        inline float b() const;

        inline const vec3& operator+();
        inline vec3 operator-();

        /** \returns element i+1, to be used on const instance; r  */
        inline float operator[](int i) const;
        /** \returns element i+1 on nonconst instance; rw */
        inline float& operator[](int i);

        /** \returns reference to vector after in-place element-wise operations */
        inline vec3& operator+=(const vec3& v2);
        inline vec3& operator-=(const vec3& v2);
        inline vec3& operator*=(const vec3& v2);
        inline vec3& operator/=(const vec3& v2);
        inline vec3& operator*=(const float t);
        inline vec3& operator/=(const float t);

        /** \returns euclidean norm of vector */
        inline float norm() const;
        
        inline float squared_norm() const;

        inline void make_unit_vector();

        float e[3];
};








#endif