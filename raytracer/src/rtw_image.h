#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "../external/stb_image.h"

#include <cstdlib>
#include <iostream>
#include <string>

/**
 * \brief Loads an image via stb_image and exposes 8-bit RGB pixel access.
 *
 * Search order assumes either:
 *   - cwd is the repo root (pathtracer/), or
 *   - cwd is raytracer/, or
 *   - RTW_IMAGES points at the images directory.
 */
class RTWImage {
public:
    RTWImage() = default;

    explicit RTWImage(const char* imageFilename) {
        const std::string filename(imageFilename);
        // Prefer RTW_IMAGES (RTIOW naming); also accept RTW_Images.
        const char* imageDir = std::getenv("RTW_IMAGES");
        if (imageDir == nullptr) {
            imageDir = std::getenv("RTW_Images");
        }

        if (imageDir && load(std::string(imageDir) + "/" + filename)) {
            return;
        }
        // cwd = repo root
        if (load("raytracer/images/" + filename)) {
            return;
        }
        // cwd = raytracer/
        if (load("images/" + filename)) {
            return;
        }
        if (load(filename)) {
            return;
        }
        if (load("../images/" + filename)) {
            return;
        }
        if (load("../../images/" + filename)) {
            return;
        }
        if (load("../raytracer/images/" + filename)) {
            return;
        }

        std::cerr << "ERROR: Could not load image file '" << imageFilename << "'.\n";
    }

    ~RTWImage() {
        delete[] bData;
        STBI_FREE(fData);
    }

    RTWImage(const RTWImage&) = delete;
    RTWImage& operator=(const RTWImage&) = delete;

    bool load(const std::string& filename) {
        auto n = bytesPerPixel;
        float* data = stbi_loadf(filename.c_str(), &imageWidth, &imageHeight, &n, bytesPerPixel);
        if (data == nullptr) {
            return false;
        }

        fData = data;
        bytesPerScanline = imageWidth * bytesPerPixel;
        convertToBytes();
        return true;
    }

    int width() const { return (fData == nullptr) ? 0 : imageWidth; }
    int height() const { return (fData == nullptr) ? 0 : imageHeight; }

    const unsigned char* pixelData(int x, int y) const {
        static unsigned char magenta[] = {255, 0, 255};
        if (bData == nullptr) {
            return magenta;
        }
        x = clamp(x, 0, imageWidth);
        y = clamp(y, 0, imageHeight);
        return bData + y * bytesPerScanline + x * bytesPerPixel;
    }

    /** Forces x into [min, max). */
    static int clamp(int x, int min, int max) {
        if (x < min) {
            return min;
        }
        if (x < max) {
            return x;
        }
        return max - 1;
    }

    static float clamp(float x, float min, float max) {
        if (x < min) {
            return min;
        }
        if (x > max) {
            return max;
        }
        return x;
    }

    static unsigned char floatToByte(float value) {
        if (value <= 0.0f) {
            return 0;
        }
        if (value >= 1.0f) {
            return 255;
        }
        return static_cast<unsigned char>(256.0f * value);
    }

    void convertToBytes() {
        const int totalBytes = imageWidth * imageHeight * bytesPerPixel;
        bData = new unsigned char[totalBytes];
        for (int i = 0; i < totalBytes; ++i) {
            bData[i] = floatToByte(fData[i]);
        }
    }

private:
    const int bytesPerPixel = 3;
    float* fData = nullptr;
    unsigned char* bData = nullptr;
    int imageWidth = 0;
    int imageHeight = 0;
    int bytesPerScanline = 0;
};
