#pragma once

#include <limits>

#if defined(__cplusplus) && __cplusplus >= 202002L && !defined(__CUDA_ARCH__)
#include <numbers>
inline constexpr float pi = std::numbers::pi_v<float>;
#else
inline constexpr float pi = 3.14159265358979323846f;
#endif

inline constexpr float infinity = std::numeric_limits<float>::infinity();

/** \brief Converts degrees to radians. */
inline float degreesToRadians(float degrees) {
    return degrees * pi / 180.0f;
}
