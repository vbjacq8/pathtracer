#pragma once

#include "vec3.h"
#include <algorithm>
#include <vector>

/** 
 *\brief framebuffer class for accumulative sampling 
 */
class Framebuffer {
    public:
        Framebuffer() : width(0), height(0) {}

        Framebuffer(int w, int h) : width(w), height(h), sum(w * h, vec3(0, 0, 0)), count(w * h, 0) {}

        /**
        *\brief reset method to clear all entries of the framebuffer */
        void reset() {
            std::fill(sum.begin(), sum.end(), vec3(0, 0, 0));
            std::fill(count.begin(), count.end(), 0);
        }

        /**
        *\brief adds a sampled color to a single pixel
        *\param x horizontal index
        *\param y veritcal index
        *\param col color vector that was sampled 
        */
        void addSample(int x, int y, const vec3& col) {
            int idx = y * width + x;
            sum[idx] += col;
            count[idx]++;
        }

        /**
        *\brief getter method that gets the averaged color for a single pixel */
        vec3 pixel(int x, int y) const {
            int idx = y * width + x;
            if (count[idx] == 0) {
                return vec3(0, 0, 0);
            }
            return sum[idx] / double(count[idx]);
        }

        /**
        *\brief getter method that gets the number of samples on a pixel */
        int samplesAt(int x, int y) const {
            return count[y * width + x];
        }

        int width;
        int height;
        //Vector where each component contains the sum of colors for a pixel soon to be averaged by count
        std::vector<vec3> sum;
        //Vector where each component contains the # of samples added to the pixel
        std::vector<int> count;
};
