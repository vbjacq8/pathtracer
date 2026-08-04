#include "../../src/materials.h"
#include "../../src/mvc/interactive.h"
#include "../../src/my_random.h"
#include "../../src/box.h"
#include "../../src/hittables.h"
#include "../../src/bvh_jacco.h"

#include <memory>
#include <vector>

HitablePtr randomScene() {
    std::vector<HitablePtr> objects;

    auto checker = new Checkered(0.32, vec3(0.2, 0.3, 0.1), vec3(0.9, 0.9, 0.9));
    objects.push_back(std::make_shared<Sphere>(
        vec3(0, -1000, 0), 1000, new Lambertian(checker)));

    for (int a = -4; a < 4; a++) {
        for (int b = -4; b < 4; b++) {
            float chooseMat = randomFloat(0.0, 1.0);
            vec3 center(a + 0.9 * randomFloat(0.0, 1.0), 0.2, b + 0.9 * randomFloat(0.0, 1.0));
            if ((center - vec3(4.0, 0.2, 0)).norm() > 0.9) {
                if (chooseMat < 0.8) {
                    auto albedo = new SolidColor(vec3(
                        randomFloat(0.0, 1.0) * randomFloat(0.0, 1.0),
                        randomFloat(0.0, 1.0) * randomFloat(0.0, 1.0),
                        randomFloat(0.0, 1.0) * randomFloat(0.0, 1.0)));
                    objects.push_back(std::make_shared<Sphere>(
                        center, 0.2, new Lambertian(albedo)));
                } else if (chooseMat < 0.95) {
                    objects.push_back(std::make_shared<Sphere>(
                        center,
                        0.2,
                        new Metal(
                            vec3(
                                0.5 * (1 + randomFloat(0.0, 1.0)),
                                0.5 * (1 + randomFloat(0.0, 1.0)),
                                0.5 * (1 + randomFloat(0.0, 1.0))),
                            0.5 * randomFloat(0.0, 1.0))));
                } else {
                    objects.push_back(std::make_shared<Sphere>(
                        center, 0.2, new Dielectric(1.5)));
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
    objects.push_back(std::make_shared<Box>(
        vec3(-2, 3, 0), vec3(-1, 4, 1), new Light(vec3(10, 200, 300))));
    objects.push_back(std::make_shared<Box>(
        vec3(0, 3, -1), vec3(1, 4, 0), new Light(vec3(400, 200, 20))));
    objects.push_back(std::make_shared<Box>(
        vec3(-4, 4, -2), vec3(-3, 5, -1), new Metal(vec3(1, 1, 1), 0.5)));
    objects.push_back(std::make_shared<Box>(
        vec3(4, 5, -6), vec3(5, 6, -5), new Dielectric(1.5)));
    objects.push_back(std::make_shared<Box>(
        vec3(-3, 3, -2),
        vec3(-2, 4, -1),
        new Lambertian(new SolidColor(vec3(0.5, 0.5, 0.5)))));

    return std::make_shared<HitableList>(std::move(objects));
}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, randomScene());
}
