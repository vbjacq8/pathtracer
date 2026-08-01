#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <cmath>
#include <stdlib.h>

#include "cuda_annot.h"

// __CUDACC__ is set by nvcc (host + device passes). __CUDA_ARCH__ is set only
// on the device pass — keep iostream / stream ops off the GPU compile.
#ifndef __CUDA_ARCH__
#include <iostream>
#endif

namespace {
    constexpr float nearZeroEps = 1e-8f;
}

/**
 * \brief Three-component vector for positions, directions, and colors.
 */
class vec3 {
public:
    PATHTRACER_HD vec3() {}
    PATHTRACER_HD vec3(float e1, float e2, float e3) { e[0] = e1; e[1] = e2; e[2] = e3; }

    PATHTRACER_HD inline float x() const { return e[0]; }
    PATHTRACER_HD inline float y() const { return e[1]; }
    PATHTRACER_HD inline float z() const { return e[2]; }
    PATHTRACER_HD inline float r() const { return e[0]; }
    PATHTRACER_HD inline float g() const { return e[1]; }
    PATHTRACER_HD inline float b() const { return e[2]; }

    PATHTRACER_HD inline const vec3& operator+() { return *this; }
    PATHTRACER_HD inline vec3 operator-() { return vec3(-e[0], -e[1], -e[2]); }

    PATHTRACER_HD inline float operator[](int i) const { return e[i]; }
    PATHTRACER_HD inline float& operator[](int i) { return e[i]; }

    PATHTRACER_HD inline vec3& operator+=(const vec3& v2);
    PATHTRACER_HD inline vec3& operator-=(const vec3& v2);
    PATHTRACER_HD inline vec3& operator*=(const vec3& v2);
    PATHTRACER_HD inline vec3& operator/=(const vec3& v2);
    PATHTRACER_HD inline vec3& operator*=(const float t);
    PATHTRACER_HD inline vec3& operator/=(const float t);

    /** \returns Euclidean length. */
    PATHTRACER_HD inline float norm() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    /** \returns Squared Euclidean length. */
    PATHTRACER_HD inline float squared_norm() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
    /** Scales this vector to unit length in place. */
    PATHTRACER_HD inline void make_unit_vector();
    /** True if all components are near 0 (uses abs; negatives are not "near zero"). */
    PATHTRACER_HD inline bool near_zero() const {
        return (fabs(e[0]) < nearZeroEps)
            && (fabs(e[1]) < nearZeroEps)
            && (fabs(e[2]) < nearZeroEps);
    }

    float e[3];
};

#ifndef __CUDA_ARCH__
inline std::istream& operator>>(std::istream& is, vec3& v) {
    is >> v.e[0] >> v.e[1] >> v.e[2];
    return is;
}

inline std::ostream& operator<<(std::ostream& os, vec3& v) {
    os << v.e[0] << " " << v.e[1] << " " << v.e[2];
    return os;
}
#endif

/** \brief Element-wise vector addition and subtraction. */
PATHTRACER_HD inline vec3 operator+(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

PATHTRACER_HD inline vec3 operator-(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

PATHTRACER_HD inline vec3 operator*(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

PATHTRACER_HD inline vec3 operator*(const float t, const vec3& v1) {
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

PATHTRACER_HD inline vec3 operator*(const vec3& v1, const float t) {
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

PATHTRACER_HD inline vec3 operator/(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

PATHTRACER_HD inline vec3 operator/(const vec3& v1, const float t) {
    return vec3(v1.e[0] / t, v1.e[1] / t, v1.e[2] / t);
}

PATHTRACER_HD inline float fastPow(float a, float b) {
    return powf(a, b);
}


PATHTRACER_HD inline vec3 operator^(const vec3& v1, const float t){
    return vec3(
        fastPow(v1[0], t),
        fastPow(v1[1], t),
        fastPow(v1[2], t));
}

/** \returns Dot product of \p v1 and \p v2. */
PATHTRACER_HD inline float dot(const vec3& v1, const vec3& v2) {
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

/** \returns Cross product of \p v1 and \p v2. */
PATHTRACER_HD inline vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3(
        (v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
        (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
        (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

/** \returns Unit vector parallel to \p v. */
PATHTRACER_HD inline vec3 unit_vector(const vec3& v) {
    return v / v.norm();
}

PATHTRACER_HD inline vec3& vec3::operator+=(const vec3& v2) {
    e[0] = e[0] + v2.e[0];
    e[1] = e[1] + v2.e[1];
    e[2] = e[2] + v2.e[2];
    return *this;
}

PATHTRACER_HD inline vec3& vec3::operator-=(const vec3& v2) {
    e[0] = e[0] - v2.e[0];
    e[1] = e[1] - v2.e[1];
    e[2] = e[2] - v2.e[2];
    return *this;
}

PATHTRACER_HD inline vec3& vec3::operator*=(const vec3& v2) {
    e[0] = e[0] * v2.e[0];
    e[1] = e[1] * v2.e[1];
    e[2] = e[2] * v2.e[2];
    return *this;
}

PATHTRACER_HD inline vec3& vec3::operator/=(const vec3& v2) {
    e[0] = e[0] / v2.e[0];
    e[1] = e[1] / v2.e[1];
    e[2] = e[2] / v2.e[2];
    return *this;
}

PATHTRACER_HD inline vec3& vec3::operator*=(const float t) {
    e[0] = e[0] * t;
    e[1] = e[1] * t;
    e[2] = e[2] * t;
    return *this;
}

PATHTRACER_HD inline vec3& vec3::operator/=(const float t) {
    e[0] = e[0] / t;
    e[1] = e[1] / t;
    e[2] = e[2] / t;
    return *this;
}

PATHTRACER_HD inline void vec3::make_unit_vector() {
    float magnitude = norm();
    *this /= magnitude;
}

PATHTRACER_HD inline vec3 min3(vec3& v1, vec3& v2){
    return vec3(
        fminf(v1[0], v2[0]),
        fminf(v1[1], v2[1]),
        fminf(v1[2], v2[2])
    );
}

PATHTRACER_HD inline vec3 max3(vec3& v1, vec3& v2){
    return vec3(
        fmaxf(v1[0], v2[0]),
        fmaxf(v1[1], v2[1]),
        fmaxf(v1[2], v2[2])
    );
}



#endif
