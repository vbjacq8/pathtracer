#pragma once

#include <limits>
#include <numbers>

/**
 * \brief Shared numeric constants and helpers (RTIOW-style).
 *
 * Prefer this header over per-file pi / infinity definitions to avoid
 * duplicate or ambiguous symbols when headers are combined.
 */
inline constexpr double pi = std::numbers::pi;
inline constexpr double infinity = std::numeric_limits<double>::infinity();

/** \brief Converts degrees to radians. */
inline double degreesToRadians(double degrees) {
    return degrees * pi / 180.0;
}
