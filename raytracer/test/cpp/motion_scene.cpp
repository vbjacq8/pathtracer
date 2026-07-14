#include "../../src/materials.h"
#include "../../src/mvc/interactive.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"

#include <memory>

HitablePtr motionScene() {
    auto world = std::make_shared<HitableList>();
    auto groundMaterial = std::make_shared<Lambertian>(vec3(0.5,0.5,0.5));
    world->add(make_shared<Sphere>(vec3(0,-1000,0), 1000, groundMaterial));

    for (int i = -4; i < 4; ++i){
        for (int j = -4; j < 4; ++j){
            auto chooseMat = randomDouble(0,1);
            vec3 cen(i + 0.9 * randomDouble(0,1), 0.2, j + 0.9 * randomDouble(0,1));
            if ((cen - vec3(4, 0.2, 0)).norm() > 0.9){
                std::shared_ptr<Material> sphereMaterial;

                if (chooseMat < 0.8){
                    sphereMaterial = std::make_shared<Lambertian>(
                        vec3(
                                randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                                randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0),
                                randomDouble(0.0, 1.0) * randomDouble(0.0, 1.0)
                        )
                    
                    );
                    auto cen2 = cen + vec3(0, randomDouble(0, 0.5), 0);
                    world->add(std::make_shared<Sphere>(cen, cen2, 0.2, sphereMaterial));
                }

                else if (chooseMat < 0.95) {
                    world->add(std::make_shared<Sphere>(
                        cen,
                        0.2,
                        std::make_shared<Metal>(
                            vec3(
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0)),
                                0.5 * (1 + randomDouble(0.0, 1.0))),
                            0.5 * randomDouble(0.0, 1.0))));
                } else {
                    world->add(std::make_shared<Sphere>(
                        cen, 0.2, std::make_shared<Dielectric>(1.5)));
                }
            }


        }
    }
    return world;

}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, motionScene());
}
