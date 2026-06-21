#include "../../src/materials.h"
#include "../../src/mvc/interactive.h"
#include "../../src/my_random.h"

Hitable* randomScene() {
    int n = 500;
    Hitable** list = new Hitable*[n + 1];
    list[0] = new Sphere(vec3(0, -1000, 0), 1000, new Lambertian(vec3(0.5, 0.5, 0.5)));
    int i = 1;
    for (int a = -4; a < 4; a++) {
        for (int b = -4; b < 4; b++) {
            double chooseMat = randomDouble(0.0, 1.0);
            vec3 center(a + 0.9 * randomDouble(0.0, 1.0), 0.2, b + 0.9 * randomDouble(0.0, 1.0));
            if ((center - vec3(4.0, 0.2, 0)).norm() > 0.9) {
                if (chooseMat < 0.8) {
                    list[i++] = new Sphere(
                        center,
                        0.2,
                        new Lambertian(vec3(
                            randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                            randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                            randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0))));
                } else if (chooseMat < 0.95) {
                    list[i++] = new Sphere(
                        center,
                        0.2,
                        new Metal(
                            vec3(
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0))),
                            0.5 * randomDouble(0.0, 1.0)));
                } else {
                    list[i++] = new Sphere(center, 0.2, new Dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new Sphere(vec3(0, 1, 0), 1.0, new Dielectric(1.5));
    list[i++] = new Sphere(vec3(-4, 1, 0), 1.0, new Lambertian(vec3(0.4, 0.2, 0.1)));
    list[i++] = new Sphere(vec3(4, 1, 0), 1.0, new Metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new HitableList(list, i);
}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, randomScene());
}
