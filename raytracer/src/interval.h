#pragma once

#include "constants.h"

#include <cmath>

class Interval {
public:
    float min, max;

    Interval() : min(+infinity), max(-infinity) {}

    Interval(float min, float max) : min(min), max(max) {}

    Interval(const Interval& a, const Interval& b) {
        min = std::fmin(a.min, b.min);
        max = std::fmax(a.max, b.max);
    }

    float length() const { return max - min; }

    bool contains(float x) const { return (x >= min) && (x <= max); }

    bool surrounds(float x) const { return (x > min) && (x < max); }

    static const Interval empty, universe;
};
