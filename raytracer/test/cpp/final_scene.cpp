#include "../../src/materials.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"
#include "../../src/bvh_weekend.h"
//#include "../../src/mvc/interactive.h"
#include "../../src/render.h"

#include <memory>
#include <vector>

/**
 * \brief RTIOW2 final scene.
 *
 * Recommended CLI args (spaces, not commas):
 *   --lookfrom 478 278 -600 --lookat 278 278 0 --vfov 40
 */
HitablePtr finalScene() {
    std::vector<HitablePtr> objects;

    auto ground = std::make_shared<Lambertian>(vec3(0.48, 0.83, 0.53));

    std::vector<HitablePtr> boxPrims;
    constexpr int boxesPerSide = 20;
    for (int i = 0; i < boxesPerSide; ++i) {
        for (int j = 0; j < boxesPerSide; ++j) {
            const float w = 100.0;
            const float x0 = -1000.0 + i * w;
            const float z0 = -1000.0 + j * w;
            const float y0 = 0.0;
            const float x1 = x0 + w;
            const float y1 = randomFloat(1, 101);
            const float z1 = z0 + w;

            boxPrims.push_back(
                std::make_shared<BoxQuadImpl>(vec3(x0, y0, z0), vec3(x1, y1, z1), ground));
        }
    }
    objects.push_back(std::make_shared<BVHWeekend>(std::move(boxPrims)));

    auto light = std::make_shared<DiffuseLight>(vec3(7, 7, 7));
    objects.push_back(std::make_shared<Quad>(vec3(123, 554, 147), vec3(300, 0, 0), vec3(0, 0, 265), light));

    const vec3 cen1(400, 400, 200);
    const vec3 cen2 = cen1 + vec3(30, 0, 0);
    auto sphereMat = std::make_shared<Lambertian>(vec3(0.7, 0.3, 0.1));
    objects.push_back(std::make_shared<Sphere>(cen1, cen2, 50, sphereMat));

    objects.push_back(std::make_shared<Sphere>(vec3(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
    objects.push_back(std::make_shared<Sphere>(vec3(0, 150, 145), 50, std::make_shared<Metal>(vec3(0.8, 0.8, 0.9), 1.0)));

    auto boundary = std::make_shared<Sphere>(vec3(360, 150, 145), 70, std::make_shared<Dielectric>(1.5));
    objects.push_back(boundary);
    objects.push_back(std::make_shared<ConstantMedium>(boundary, 0.2, vec3(0.2, 0.4, 0.9)));
    boundary = std::make_shared<Sphere>(vec3(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
    objects.push_back(std::make_shared<ConstantMedium>(boundary, 0.0001, vec3(1, 1, 1)));

    auto emat = std::make_shared<Lambertian>(std::make_shared<Wallpaper>("earthmap.jpg"));
    objects.push_back(std::make_shared<Sphere>(vec3(400, 200, 400), 100, emat));
    auto pertext = std::make_shared<Noise>(0.2);
    objects.push_back(std::make_shared<Sphere>(vec3(220, 280, 300), 80, std::make_shared<Lambertian>(pertext)));

    std::vector<HitablePtr> boxPrims2;
    auto white = std::make_shared<Lambertian>(vec3(0.73, 0.73, 0.73));
    constexpr int ns = 1000;
    for (int j = 0; j < ns; ++j) {
        boxPrims2.push_back(std::make_shared<Sphere>(
            vec3(165 * randomFloat(0, 1), 165 * randomFloat(0, 1), 165 * randomFloat(0, 1)),
            10,
            white));
    }

    objects.push_back(std::make_shared<Translate>(
        std::make_shared<RotateY>(std::make_shared<BVHWeekend>(std::move(boxPrims2)), 15),
        vec3(-100, 270, 395)));

    return std::make_shared<BVHWeekend>(std::move(objects));
}

int main(int argc, char** argv) {
    return render(argc, argv, finalScene(), colorVoid);
}
