#include "../../src/materials.h"
#include "../../src/mvc/interactive.h"
#include "../../src/my_random.h"
#include "../../src/bvh_jacco.h"
#include "../../src/hittables.h"

#include <memory>
#include <vector>

HitablePtr randomScene() {
    std::vector<HitablePtr> objects;

    auto checker = std::make_shared<Checkered>(0.32, vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
    objects.push_back(std::make_shared<Sphere>(
        vec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            double chooseMat = randomDouble(0.0, 1.0);
            vec3 center(a + 0.9 * randomDouble(0.0, 1.0), 0.2, b + 0.9 * randomDouble(0.0, 1.0));
            if ((center - vec3(4.0, 0.2, 0)).norm() > 0.9) {
                if (chooseMat < 0.8) {
                    auto albedo = std::make_shared<SolidColor>(vec3(
                        randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                        randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                        randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0)));
                    objects.push_back(std::make_shared<Sphere>(
                        center, 0.2, std::make_shared<Lambertian>(albedo)));
                } else if (chooseMat < 0.95) {
                    objects.push_back(std::make_shared<Sphere>(
                        center,
                        0.2,
                        std::make_shared<Metal>(
                            vec3(
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0))),
                            0.5 * randomDouble(0.0, 1.0))));
                } else {
                    objects.push_back(std::make_shared<Sphere>(
                        center, 0.2, std::make_shared<Dielectric>(1.5)));
                }
            }
        }
    }

    objects.push_back(std::make_shared<Sphere>(
        vec3(0, 1, 0), 1.0, std::make_shared<Dielectric>(1.5)));
    objects.push_back(std::make_shared<Sphere>(
        vec3(-4, 1, 0),
        1.0,
        std::make_shared<Lambertian>(std::make_shared<SolidColor>(vec3(0.4, 0.2, 0.1)))));
    objects.push_back(std::make_shared<Sphere>(
        vec3(4, 1, 0), 1.0, std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0)));

    return std::make_shared<BVHJacco>(std::move(objects));
}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, randomScene());
}
