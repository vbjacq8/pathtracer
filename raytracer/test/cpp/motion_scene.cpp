#include "../../src/materials.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"
#include "../../src/bvh_weekend.h"
//#include "../../src/bvh_jacco.h"
#include "../../src/mvc/interactive.h"

#include <memory>
#include <vector>

HitablePtr motionScene() {
    std::vector<HitablePtr> objects;

    auto checker = new Checkered(0.32, vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
    objects.push_back(std::make_shared<Sphere>(
        vec3(0, -1000, 0), 1000, new Lambertian(checker)));

    for (int i = -11; i < 11; ++i) {
        for (int j = -11; j < 11; ++j) {
            auto chooseMat = randomFloat(0, 1);
            vec3 cen(i + 0.9 * randomFloat(0, 1), 0.2, j + 0.9 * randomFloat(0, 1));
            if ((cen - vec3(4, 0.2, 0)).norm() > 0.9) {
                if (chooseMat < 0.8) {
                    auto albedo = new SolidColor(vec3(
                        randomFloat(0.0, 1.0) * randomFloat(0.0, 1.0),
                        randomFloat(0.0, 1.0) * randomFloat(0.0, 1.0),
                        randomFloat(0.0, 1.0) * randomFloat(0.0, 1.0)));
                    auto cen2 = cen + vec3(0, randomFloat(0, 0.5), 0);
                    objects.push_back(std::make_shared<Sphere>(
                        cen, cen2, 0.2, new Lambertian(albedo)));
                } else if (chooseMat < 0.95) {
                    objects.push_back(std::make_shared<Sphere>(
                        cen,
                        0.2,
                        new Metal(
                            vec3(
                                0.5 * (1 + randomFloat(0.0, 1.0)),
                                0.5 * (1 + randomFloat(0.0, 1.0)),
                                0.5 * (1 + randomFloat(0.0, 1.0))),
                            0.5 * randomFloat(0.0, 1.0))));
                } else {
                    objects.push_back(std::make_shared<Sphere>(
                        cen, 0.2, new Dielectric(1.5)));
                }
            }
        }
    }

    objects.push_back(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0, new Dielectric(1.5)));

    objects.push_back(std::make_shared<Sphere>(
        vec3(-4, 1, 0),
        1.0,
        new Lambertian(new SolidColor(vec3(0.4, 0.2, 0.1)))));
    objects.push_back(std::make_shared<Sphere>(
        vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0)));
    
    objects.push_back(std::make_shared<Sphere>(vec3(0,3,-2), 1.0, new Lambertian(new Wallpaper("earthmap.jpg"))));
    objects.push_back(std::make_shared<Box>(
        vec3(-2, 3, 0), vec3(-1, 4, 1), new Light(vec3(10, 200, 300))));
    objects.push_back(std::make_shared<Box>(
        vec3(0, 3, -1), vec3(1, 4, 0), new Light(vec3(400, 200, 20))));

    /**
    
    objects.push_back(std::make_shared<Box>(
        vec3(-4, 4, -2), vec3(-3, 5, -1), new Metal(vec3(1, 1, 1), 0.5)));
    objects.push_back(std::make_shared<Box>(
        vec3(4, 5, -6), vec3(5, 6, -5), new Dielectric(1.5)));
    objects.push_back(std::make_shared<Box>(
        vec3(-3, 3, -2),
        vec3(-2, 4, -1),
        new Lambertian(new SolidColor(vec3(0.5, 0.5, 0.5)))));

    */


    return std::make_shared<BVHWeekend>(std::move(objects));
}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, motionScene());
}
