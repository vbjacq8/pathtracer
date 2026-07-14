#pragma once
#include<cmath>

class Interval {
    public:
        double min, max;
        Interval() : min(std::numeric_limits<double>::infinity())
            , max(-std::numeric_limits<double>::infinity()) {}
        
        Interval(double min, double max) : min(min), max(max) {}

        Interval(const Interval& a, const Interval& b){
            min = std::fminf(a.min, b.min);
            max = std::fmaxf(a.max,b.max);
        }

        double length() const {return max - min;}

        bool contains(double x) const {
            return (
                (x >= min) && (x <= max)
            );
        }

        bool surrounds(double x) const {
            return (
                (x > min) && (x < max)
            );
        }



        static const Interval empty, universe;

};