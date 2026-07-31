#pragma once

#include <random>
#include <cmath>

#include "constants.h"
#include "vec3.h"

/**
 * \brief Uniform random numbers in [\p min, \p max).
 * \param min inclusive lower bound
 * \param max exclusive upper bound
 */
inline float randomFloat(float min, float max) {
    static std::mt19937 engine{std::random_device{}()};
    static std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return min + (max - min) * dist(engine);
}

inline int randomInt(int min, int max) {
    // Inclusive range [min, max], matching typical RTIOW helper usage.
    static std::mt19937 engine{std::random_device{}()};
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine);
}

/**
 * \brief Random point inside the unit sphere
 */
inline vec3 randomInSphere() {
    float u = randomFloat(0.0f, 1.0f);
    float v = randomFloat(0.0f, 1.0f);
    float w = randomFloat(0.0f, 1.0f);
    float theta = 2.0f * pi * u;
    float phi = std::acos(2.0f * v - 1.0f);  // uniform on sphere
    float r = std::cbrt(w);                  // cbrt, not sqrt — volume element
    float s = std::sin(phi);
    return vec3(r * s * std::cos(theta), r * s * std::sin(theta), r * std::cos(phi));
}

/**
 * \brief Random point inside the unit disc in the xy-plane.
 */
inline vec3 randomInDisc() {
    float u = randomFloat(0.0f, 1.0f);
    float v = randomFloat(0.0f, 1.0f);
    float theta = 2.0f * pi * u;
    float r = std::sqrt(v);
    return vec3(r * std::cos(theta), r * std::sin(theta), 0.0f);
}
