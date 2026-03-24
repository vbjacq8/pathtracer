#include"vec3.h"

class vec3{
    public:
        vec3(){}
        vec3(float e1, float e2, float e3){e[0] = e1; e[1] = e2; e[2] = e3;}
        inline float x() const {return e[0];}
        inline float y() const {return e[1];}
        inline float z() const {return e[2];}
        inline float r() const {return e[0];}
        inline float g() const {return e[1];}
        inline float b() const {return e[2];}

        inline const vec3& operator+(){return *this;}
        inline vec3 operator-(){return vec3(-e[0], -e[1], -e[2]);}
        inline float operator[](int i) const {return e[i];}
        inline float& operator[](int i){return e[i];}

        inline vec3& operator+=(const vec3& v2){
            e[0] = e[0] + v2.e[0];
            e[1] = e[1] + v2.e[1];
            e[2] = e[2] + v2.e[2];
            return *this;
        }

        inline vec3& operator-=(const vec3& v2){
            e[0] = e[0] - v2.e[0];
            e[1] = e[1] - v2.e[1];
            e[2] = e[2] - v2.e[2];
            return *this;
        }

        inline vec3& operator*=(const vec3& v2){
            e[0] = e[0] * v2.e[0];
            e[1] = e[1] * v2.e[1];
            e[2] = e[2] * v2.e[2];
            return *this;
        }
        inline vec3& operator/=(const vec3& v2){
            e[0] = e[0] / v2.e[0];
            e[1] = e[1] / v2.e[1];
            e[2] = e[2] / v2.e[2];
            return *this;
        }

        inline vec3& operator*=(const float t){
            e[0] = e[0] * t;
            e[1] = e[1] * t;
            e[2] = e[2] * t;
            return *this;
        }

        inline vec3& operator/=(const float t){
            e[0] = e[0] / t;
            e[1] = e[1] / t;
            e[2] = e[2] / t;
            return *this;
        }

        inline float norm() const {
            return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
        }

        inline float squared_norm() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }
        inline void make_unit_vector() {
            float norm = (*this).norm();
            e[0] /= norm;
            e[1] /= norm;
            e[2] /= norm;
        }

        float e[3];
};
