#include "../../src/materials.h"
#include "../../src/mvc/interactive.h"
#include "../../src/my_random.h"
#include "../../src/box.h"
#include "../../src/hittables.h"

#include <memory>

HitablePtr randomScene() {
    auto world = std::make_shared<HitableList>();
    world->add(std::make_shared<Sphere>(
        vec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(vec3(0.5, 0.5, 0.5))));

    for (int a = -4; a < 4; a++) {
        for (int b = -4; b < 4; b++) {
            double chooseMat = randomDouble(0.0, 1.0);
            vec3 center(a + 0.9 * randomDouble(0.0, 1.0), 0.2, b + 0.9 * randomDouble(0.0, 1.0));
            if ((center - vec3(4.0, 0.2, 0)).norm() > 0.9) {
                if (chooseMat < 0.8) {
                    world->add(std::make_shared<Sphere>(
                        center,
                        0.2,
                        std::make_shared<Lambertian>(vec3(
                            randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                            randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                            randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0)))));
                } else if (chooseMat < 0.95) {
                    world->add(std::make_shared<Sphere>(
                        center,
                        0.2,
                        std::make_shared<Metal>(
                            vec3(
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0))),
                            0.5 * randomDouble(0.0, 1.0))));
                } else {
                    world->add(std::make_shared<Sphere>(
                        center, 0.2, std::make_shared<Dielectric>(1.5)));
                }
            }
        }
    }

    world->add(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0, std::make_shared<Dielectric>(1.5)));
    world->add(std::make_shared<Sphere>(
        vec3(-4, 1, 0), 1.0, std::make_shared<Lambertian>(vec3(0.4, 0.2, 0.1))));
    world->add(std::make_shared<Sphere>(
        vec3(4, 1, 0), 1.0, std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0)));
    world->add(std::make_shared<Box>(
        vec3(-2, 3, 0), vec3(-1, 4, 1), std::make_shared<Light>(vec3(10, 200, 300))));
    world->add(std::make_shared<Box>(
        vec3(0, 3, -1), vec3(1, 4, 0), std::make_shared<Light>(vec3(400, 200, 20))));
    world->add(std::make_shared<Box>(
        vec3(-4, 4, -2), vec3(-3, 5, -1), std::make_shared<Metal>(vec3(1, 1, 1), 0.5)));
    world->add(std::make_shared<Box>(
        vec3(4, 5, -6), vec3(5, 6, -5), std::make_shared<Dielectric>(1.5)));
    world->add(std::make_shared<Box>(
        vec3(-3, 3, -2), vec3(-2, 4, -1), std::make_shared<Lambertian>(vec3(0.5, 0.5, 0.5))));

    return world;
}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, randomScene());
}
