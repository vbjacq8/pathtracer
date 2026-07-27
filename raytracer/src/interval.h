#pragma once

#include "constants.h"

#include <cmath>

class Interval {
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {}

    Interval(double min, double max) : min(min), max(max) {}

    Interval(const Interval& a, const Interval& b) {
        min = std::fmin(a.min, b.min);
        max = std::fmax(a.max, b.max);
    }

    double length() const { return max - min; }

    bool contains(double x) const { return (x >= min) && (x <= max); }

    bool surrounds(double x) const { return (x > min) && (x < max); }

    static const Interval empty, universe;
};
