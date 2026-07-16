#pragma once

#include <random>
#include "vec3.h"

/**
 * \brief Uniform random numbers in [\p min, \p max).
 * \param min inclusive lower bound
 * \param max exclusive upper bound
 */
inline double randomDouble(double min, double max) {
    static std::mt19937_64 engine{std::random_device{}()};
    static std::uniform_real_distribution<> dist(0.0, 1.0);
    return min + (max - min) * dist(engine);
}

inline double randomInt(int min, int max){
    static std::mt19937_64 engine{std::random_device{}()};
    static std::uniform_int_distribution<> dist(0, 1);
    return min + (max - min) * dist(engine);
}


/**
 * \brief Random point inside the unit sphere
 */
inline vec3 randomInSphere() {
    double u = randomDouble(0, 1);
    double v = randomDouble(0, 1);
    double w = randomDouble(0, 1);
    double theta = 2 * M_PI * u;
    double phi = std::acos(2 * v - 1);   // uniform on sphere
    double r = std::cbrt(w);             // cbrt, not sqrt — volume element
    double s = std::sin(phi);
    return vec3(r * s * std::cos(theta), r * s * std::sin(theta), r * std::cos(phi));
}

/**
 * \brief Random point inside the unit disc in the xy-plane.
 */
inline vec3 randomInDisc() {
    double u = randomDouble(0,1);
    double v = randomDouble(0,1);
    double theta = 2*M_PI * u;
    double r = std::sqrt(v);
    return vec3(r * std::cos(theta), r * std::sin(theta), 0);
}
