#pragma once

#include "vec3.h"
#include "rtw_image.h"
#include "perlin.h"

#include <cmath>
#include <memory>


/**
 * \brief Texture that maps hit parameters to a color.
 * Hit location comes from Hitable::hit; the texture samples color at that point.
 */
class Texture {
    public:
        virtual ~Texture() = default;
        virtual vec3 value(double u, double v, const vec3& p) const = 0;
};

/** \brief Constant color for all coordinates. */
class SolidColor : public Texture {
    public:
        SolidColor(const vec3& a) : albedo(a) {}
        SolidColor(double r, double g, double b) : SolidColor(vec3(r, g, b)) {}

        vec3 value(double /*u*/, double /*v*/, const vec3& /*p*/) const override {
            return albedo;
        }

    private:
        vec3 albedo;
};

/** \brief Spatial checkerboard using even/odd solid (or nested) textures. */
class Checkered : public Texture {
    public:
        Checkered(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
            : invScale(1.0 / scale), even(std::move(even)), odd(std::move(odd)) {}

        Checkered(double scale, const vec3& evenColor, const vec3& oddColor)
            : Checkered(scale, std::make_shared<SolidColor>(evenColor), std::make_shared<SolidColor>(oddColor)) {}

        vec3 value(double u, double v, const vec3& p) const override {
            const int x = static_cast<int>(std::floor(invScale * p.x()));
            const int y = static_cast<int>(std::floor(invScale * p.y()));
            const int z = static_cast<int>(std::floor(invScale * p.z()));
            if ((x + y + z) % 2 == 0) {
                return even->value(u, v, p);
            }
            return odd->value(u, v, p);
        }

    private:
        double invScale;
        std::shared_ptr<Texture> even;
        std::shared_ptr<Texture> odd;
};

/** \brief Image texture mapped via u,v coordinates */
class Wallpaper : public Texture {
    public: 
        Wallpaper(const char* filename) : image(filename) {}
        vec3 value(double u, double v, const vec3& /*p*/) const override {
            if (image.height() <= 0) {
                return vec3(0, 1, 1);  // cyan = missing texture
            }

            u = RTWImage::clamp(static_cast<float>(u), 0.0f, 1.0f);
            v = 1.0f - RTWImage::clamp(static_cast<float>(v), 0.0f, 1.0f);  // flip v to image coords

            const int i = static_cast<int>(u * image.width());
            const int j = static_cast<int>(v * image.height());
            const unsigned char* pixel = image.pixelData(i, j);
            constexpr double colorScale = 1.0 / 255.0;
            return colorScale * vec3(pixel[0], pixel[1], pixel[2]);
        }

    private:
        RTWImage image;        

};

/** \brief Perlin noise texture */
class Noise : public Texture {
    public: 
        Noise() {}

        vec3 value(double u, double v, const vec3& p) const override {
            return vec3(1,1,1) * perlin.noise(p);
        }

    private:
        Perlin perlin;

};
