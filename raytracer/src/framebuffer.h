#pragma once

#include "vec3.h"
#include <algorithm>
#include <vector>

/**
 * \brief Accumulating render target that averages samples per pixel over time.
 */
class Framebuffer {
public:
    Framebuffer() : width(0), height(0) {}

    Framebuffer(int w, int h) : width(w), height(h), sum(w * h, vec3(0, 0, 0)), count(w * h, 0) {}

    /** \brief Clears all accumulated samples. */
    void reset() {
        std::fill(sum.begin(), sum.end(), vec3(0, 0, 0));
        std::fill(count.begin(), count.end(), 0);
    }

    /**
     * \brief Adds one color sample to a pixel.
     * \param x horizontal index
     * \param y vertical index
     * \param col sampled radiance
     */
    void addSample(int x, int y, const vec3& col) {
        int idx = y * width + x;
        sum[idx] += col;
        count[idx]++;
    }

    /**
     * \brief Returns the averaged color at a pixel.
     * \returns black when no samples have been accumulated
     */
    vec3 pixel(int x, int y) const {
        int idx = y * width + x;
        if (count[idx] == 0) {
            return vec3(0, 0, 0);
        }
        return sum[idx] / double(count[idx]);
    }

    /** \returns Number of samples accumulated at a pixel. */
    int samplesAt(int x, int y) const {
        return count[y * width + x];
    }

    int width;
    int height;
    std::vector<vec3> sum;   ///< Per-pixel color sums
    std::vector<int> count;  ///< Per-pixel sample counts
};
