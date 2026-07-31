#pragma once

#include "vec3.h"
#include <algorithm>
#include <cmath>

/**
 * \brief Reinhard tonemap: maps linear radiance to display-referred [0, 1] per channel.
 */
inline vec3 toneMap(vec3 radiance) {
    return radiance / (vec3(1, 1, 1) + radiance);
}

inline float toneMap(float radiance) {
    return radiance / (1 + radiance);
}

/** \brief Applies gamma encoding to display-referred values in [0, 1]. */
inline vec3 gammaCorrect(vec3 display, float gamma = 2.2f) {
    return operator^(display, 1.0f / gamma);
}

inline float gammaCorrect(float display, float gamma = 2.2f) {
    display = std::max(0.0f, std::min(display, 1.0f));
    return fastPow(display, 1.0f / gamma);
}

/** \brief Tonemap and gamma-encode linear radiance for display output. */
inline vec3 radianceToDisplay(vec3 radiance, float gamma) {
    return gammaCorrect(toneMap(radiance), gamma);
}
