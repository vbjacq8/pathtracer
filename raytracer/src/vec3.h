#ifndef VEC3_H
#define VEC3_H

#include<math.h>
#include<iostream>
#include<stdlib.h>

class vec3{
    public:
        vec3(){}
        vec3(float e1, float e2, float e3){e[0] = e1; e[1] = e2; e[2] = e3;}
        /** \brief getters for coordinates or elements */
        inline float x() const {return e[0];}
        inline float y() const {return e[1];}
        inline float z() const {return e[2];}
        inline float r() const {return e[0];}
        inline float g() const {return e[1];}
        inline float b() const {return e[2];}

        /** \brief getter for instance */
        inline const vec3& operator+(){return *this;}

        /** \brief negation operator */
        inline vec3 operator-(){return vec3(-e[0], -e[1], -e[2]);}

        /** indexing of constant vector */
        inline double operator[](int i) const {return e[i];}
        inline double& operator[](int i){return e[i];}

        /** \brief operators for element-wise relative reassignment */
        inline vec3& operator+=(const vec3& v2);
        inline vec3& operator-=(const vec3& v2);
        inline vec3& operator*=(const vec3& v2);
        inline vec3& operator/=(const vec3& v2);
        inline vec3& operator*=(const double t);
        inline vec3& operator/=(const double t);

        /** \brief Euclidian norm measures */
        inline double norm() const {return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);}
        inline double squared_norm() const {return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];}


        /** \brief scales vector by inverse of norm to make new norm 1 */
        inline void make_unit_vector();

        double e[3];
};

inline std::istream& operator>>(std::istream& is, vec3& v){
    is >> v.e[0] >> v.e[1] >> v.e[2];
    return is;
}

inline std::ostream& operator<<(std::ostream& os, vec3& v){
    os << v.e[0] << " " << v.e[1] << " " << v.e[2];
    return os;
}


/** \brief element wise vector-vector operations */
inline vec3 operator+(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator*(const double t, const vec3& v1){
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

inline vec3 operator*(const vec3& v1, const double t){
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

inline vec3 operator/(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator/(const vec3& v1, const double t) {
    return vec3(v1.e[0] / t, v1.e[1] / t, v1.e[2] / t);
}

/** \brief scalar and vector products */
inline double dot(const vec3& v1, const vec3& v2){
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3( (v1.e[1]*v2.e[2] - v1.e[2]*v2.e[1]),
    (-(v1.e[0]*v2.e[2] - v1.e[2]*v2.e[0])),
    (v1.e[0]*v2.e[1] - v1.e[1] *v2.e[0]));
}

/** \brief creates unit vector colinear with \param v1 */

inline vec3 unit_vector(const vec3& v1){
    return v1 / v1.norm();
}

/** \brief creates a vector that maps unit vector to range \param min min range \param max max range*/
//inline vec3 map_unit_vector(const vec3& v1, float min, float max){
//    vec3 minVec(min, min, min);
//   minVec+=v1;
//}

/** \brief implementations of operators on class instance */

inline vec3& vec3::operator+=(const vec3& v2){
    e[0] = e[0] + v2.e[0];
    e[1] = e[1] + v2.e[1];
    e[2] = e[2] + v2.e[2];
    return *this;
}

/** \returns vec3 instance after element wise relative reassignmnt */
inline vec3& vec3::operator-=(const vec3& v2){
    e[0] = e[0] - v2.e[0];
    e[1] = e[1] - v2.e[1];
    e[2] = e[2] - v2.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const vec3& v2){
    e[0] = e[0] * v2.e[0];
    e[1] = e[1] * v2.e[1];
    e[2] = e[2] * v2.e[2];
    return *this;
}

inline vec3& vec3::operator/=(const vec3& v2){
    e[0] = e[0] / v2.e[0];
    e[1] = e[1] / v2.e[1];
    e[2] = e[2] / v2.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const double t){
    e[0] = e[0] * t;
    e[1] = e[1] * t;
    e[2] = e[2] * t;
    return *this;
}

inline vec3& vec3::operator/=(const double t){
    e[0] = e[0] / t;
    e[1] = e[1] / t;
    e[2] = e[2] / t;
    return *this;
}

inline void vec3::make_unit_vector(){
    double magnitude = norm();
    *this /= magnitude;
}

//Utility functions




#endif
