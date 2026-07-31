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
     * \brief Adds one radiance sample to a pixel.
     * \param x horizontal index
     * \param y vertical index
     * \param radiance sampled linear radiance
     */
    void addSample(int x, int y, const vec3& radiance) {
        int idx = y * width + x;
        sum[idx] += radiance;
        count[idx]++;
    }

    /**
     * \brief Returns averaged linear radiance at a pixel.
     * \returns zero radiance when no samples have been accumulated
     */
    vec3 pixel(int x, int y) const {
        int idx = y * width + x;
        if (count[idx] == 0) {
            return vec3(0, 0, 0);
        }
        return sum[idx] / float(count[idx]);
    }

    /** \returns Number of samples accumulated at a pixel. */
    int samplesAt(int x, int y) const {
        return count[y * width + x];
    }

    int width;
    int height;
    std::vector<vec3> sum;   ///< Per-pixel linear radiance sums
    std::vector<int> count;  ///< Per-pixel sample counts
};
