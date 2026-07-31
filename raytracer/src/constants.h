#pragma once

#include <limits>
#include <numbers>

/**
 * \brief Shared numeric constants and helpers (RTIOW-style).
 *
 * Prefer this header over per-file pi / infinity definitions to avoid
 * duplicate or ambiguous symbols when headers are combined.
 */
inline constexpr float pi = std::numbers::pi_v<float>;
inline constexpr float infinity = std::numeric_limits<float>::infinity();

/** \brief Converts degrees to radians. */
inline float degreesToRadians(float degrees) {
    return degrees * pi / 180.0f;
}
