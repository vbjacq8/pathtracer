#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <cmath>
#include <stdlib.h>

// __CUDACC__ is set by nvcc (host + device passes). __CUDA_ARCH__ is set only
// on the device pass — keep iostream / stream ops off the GPU compile.
#ifndef __CUDA_ARCH__
#include <iostream>
#endif

// __CUDACC__ is defined automatically when the file is compiled by nvcc
#ifdef __CUDACC__
#define VEC3_HD __host__ __device__
#else
#define VEC3_HD
#endif

namespace {
    constexpr double nearZeroEps = 1e-8;
}

/**
 * \brief Three-component vector for positions, directions, and colors.
 */
class vec3 {
public:
    VEC3_HD vec3() {}
    VEC3_HD vec3(double e1, double e2, double e3) { e[0] = e1; e[1] = e2; e[2] = e3; }

    VEC3_HD inline double x() const { return e[0]; }
    VEC3_HD inline double y() const { return e[1]; }
    VEC3_HD inline double z() const { return e[2]; }
    VEC3_HD inline double r() const { return e[0]; }
    VEC3_HD inline double g() const { return e[1]; }
    VEC3_HD inline double b() const { return e[2]; }

    VEC3_HD inline const vec3& operator+() { return *this; }
    VEC3_HD inline vec3 operator-() { return vec3(-e[0], -e[1], -e[2]); }

    VEC3_HD inline double operator[](int i) const { return e[i]; }
    VEC3_HD inline double& operator[](int i) { return e[i]; }

    VEC3_HD inline vec3& operator+=(const vec3& v2);
    VEC3_HD inline vec3& operator-=(const vec3& v2);
    VEC3_HD inline vec3& operator*=(const vec3& v2);
    VEC3_HD inline vec3& operator/=(const vec3& v2);
    VEC3_HD inline vec3& operator*=(const double t);
    VEC3_HD inline vec3& operator/=(const double t);

    /** \returns Euclidean length. */
    VEC3_HD inline double norm() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    /** \returns Squared Euclidean length. */
    VEC3_HD inline double squared_norm() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
    /** Scales this vector to unit length in place. */
    VEC3_HD inline void make_unit_vector();
    /** True if all components are near 0 (uses abs; negatives are not "near zero"). */
    VEC3_HD inline bool near_zero() const {
        return (fabs(e[0]) < nearZeroEps)
            && (fabs(e[1]) < nearZeroEps)
            && (fabs(e[2]) < nearZeroEps);
    }

    double e[3];
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
VEC3_HD inline vec3 operator+(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

VEC3_HD inline vec3 operator-(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

VEC3_HD inline vec3 operator*(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

VEC3_HD inline vec3 operator*(const double t, const vec3& v1) {
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

VEC3_HD inline vec3 operator*(const vec3& v1, const double t) {
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

VEC3_HD inline vec3 operator/(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

VEC3_HD inline vec3 operator/(const vec3& v1, const double t) {
    return vec3(v1.e[0] / t, v1.e[1] / t, v1.e[2] / t);
}

VEC3_HD inline double fastPow(double a, double b) {
    union {
      double d;
      int x[2];
    } u = { a };
    u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
    u.x[0] = 0;
    return u.d;
  }


VEC3_HD inline vec3 operator^(const vec3& v1, const double t){
    return vec3(
        fastPow(v1[0], t),
        fastPow(v1[1], t),
        fastPow(v1[2], t));
}

/** \returns Dot product of \p v1 and \p v2. */
VEC3_HD inline double dot(const vec3& v1, const vec3& v2) {
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

/** \returns Cross product of \p v1 and \p v2. */
VEC3_HD inline vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3(
        (v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
        (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
        (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

/** \returns Unit vector parallel to \p v. */
VEC3_HD inline vec3 unit_vector(const vec3& v) {
    return v / v.norm();
}

VEC3_HD inline vec3& vec3::operator+=(const vec3& v2) {
    e[0] = e[0] + v2.e[0];
    e[1] = e[1] + v2.e[1];
    e[2] = e[2] + v2.e[2];
    return *this;
}

VEC3_HD inline vec3& vec3::operator-=(const vec3& v2) {
    e[0] = e[0] - v2.e[0];
    e[1] = e[1] - v2.e[1];
    e[2] = e[2] - v2.e[2];
    return *this;
}

VEC3_HD inline vec3& vec3::operator*=(const vec3& v2) {
    e[0] = e[0] * v2.e[0];
    e[1] = e[1] * v2.e[1];
    e[2] = e[2] * v2.e[2];
    return *this;
}

VEC3_HD inline vec3& vec3::operator/=(const vec3& v2) {
    e[0] = e[0] / v2.e[0];
    e[1] = e[1] / v2.e[1];
    e[2] = e[2] / v2.e[2];
    return *this;
}

VEC3_HD inline vec3& vec3::operator*=(const double t) {
    e[0] = e[0] * t;
    e[1] = e[1] * t;
    e[2] = e[2] * t;
    return *this;
}

VEC3_HD inline vec3& vec3::operator/=(const double t) {
    e[0] = e[0] / t;
    e[1] = e[1] / t;
    e[2] = e[2] / t;
    return *this;
}

VEC3_HD inline void vec3::make_unit_vector() {
    double magnitude = norm();
    *this /= magnitude;
}

VEC3_HD inline vec3 min3(vec3& v1, vec3& v2){
    return vec3(
        fminf(v1[0], v2[0]),
        fminf(v1[1], v2[1]),
        fminf(v1[2], v2[2])
    );
}

VEC3_HD inline vec3 max3(vec3& v1, vec3& v2){
    return vec3(
        fmaxf(v1[0], v2[0]),
        fmaxf(v1[1], v2[1]),
        fmaxf(v1[2], v2[2])
    );
}



#endif
