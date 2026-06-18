#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <math.h>
#include <stdlib.h>

/**
 * \brief Three-component vector for positions, directions, and colors.
 */
class vec3 {
public:
    vec3() {}
    vec3(double e1, double e2, double e3) { e[0] = e1; e[1] = e2; e[2] = e3; }

    inline double x() const { return e[0]; }
    inline double y() const { return e[1]; }
    inline double z() const { return e[2]; }
    inline double r() const { return e[0]; }
    inline double g() const { return e[1]; }
    inline double b() const { return e[2]; }

    inline const vec3& operator+() { return *this; }
    inline vec3 operator-() { return vec3(-e[0], -e[1], -e[2]); }

    inline double operator[](int i) const { return e[i]; }
    inline double& operator[](int i) { return e[i]; }

    inline vec3& operator+=(const vec3& v2);
    inline vec3& operator-=(const vec3& v2);
    inline vec3& operator*=(const vec3& v2);
    inline vec3& operator/=(const vec3& v2);
    inline vec3& operator*=(const double t);
    inline vec3& operator/=(const double t);

    /** \returns Euclidean length. */
    inline double norm() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    /** \returns Squared Euclidean length. */
    inline double squared_norm() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }

    /** Scales this vector to unit length in place. */
    inline void make_unit_vector();

    double e[3];
};

inline std::istream& operator>>(std::istream& is, vec3& v) {
    is >> v.e[0] >> v.e[1] >> v.e[2];
    return is;
}

inline std::ostream& operator<<(std::ostream& os, vec3& v) {
    os << v.e[0] << " " << v.e[1] << " " << v.e[2];
    return os;
}

/** \brief Element-wise vector addition and subtraction. */
inline vec3 operator+(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

inline vec3 operator-(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

inline vec3 operator*(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

inline vec3 operator*(const double t, const vec3& v1) {
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

inline vec3 operator*(const vec3& v1, const double t) {
    return vec3(v1.e[0] * t, v1.e[1] * t, v1.e[2] * t);
}

inline vec3 operator/(const vec3& v1, const vec3& v2) {
    return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

inline vec3 operator/(const vec3& v1, const double t) {
    return vec3(v1.e[0] / t, v1.e[1] / t, v1.e[2] / t);
}

/** \returns Dot product of \p v1 and \p v2. */
inline double dot(const vec3& v1, const vec3& v2) {
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

/** \returns Cross product of \p v1 and \p v2. */
inline vec3 cross(const vec3& v1, const vec3& v2) {
    return vec3(
        (v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
        (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
        (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

/** \returns Unit vector parallel to \p v. */
inline vec3 unit_vector(const vec3& v) {
    return v / v.norm();
}

inline vec3& vec3::operator+=(const vec3& v2) {
    e[0] = e[0] + v2.e[0];
    e[1] = e[1] + v2.e[1];
    e[2] = e[2] + v2.e[2];
    return *this;
}

inline vec3& vec3::operator-=(const vec3& v2) {
    e[0] = e[0] - v2.e[0];
    e[1] = e[1] - v2.e[1];
    e[2] = e[2] - v2.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const vec3& v2) {
    e[0] = e[0] * v2.e[0];
    e[1] = e[1] * v2.e[1];
    e[2] = e[2] * v2.e[2];
    return *this;
}

inline vec3& vec3::operator/=(const vec3& v2) {
    e[0] = e[0] / v2.e[0];
    e[1] = e[1] / v2.e[1];
    e[2] = e[2] / v2.e[2];
    return *this;
}

inline vec3& vec3::operator*=(const double t) {
    e[0] = e[0] * t;
    e[1] = e[1] * t;
    e[2] = e[2] * t;
    return *this;
}

inline vec3& vec3::operator/=(const double t) {
    e[0] = e[0] / t;
    e[1] = e[1] / t;
    e[2] = e[2] / t;
    return *this;
}

inline void vec3::make_unit_vector() {
    double magnitude = norm();
    *this /= magnitude;
}

#endif
