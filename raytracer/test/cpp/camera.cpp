#include "../../src/materials.h"
#include "../../src/render.h"
#include "../../src/hittables.h"

#include <memory>

HitablePtr cameraScene() {
    auto world = std::make_shared<HitableList>();
    world->add(std::make_shared<Sphere>(
        vec3(0, 0, 0), 1, new Metal(vec3(0.9, 0.9, 0.9), 0.1)));
    world->add(std::make_shared<Sphere>(
        vec3(0, 1, 0),
        1,
        new Lambertian(new SolidColor(vec3(0.5, 0.5, 0.5)))));
    return world;
}

int main(int argc, char** argv) {
    return render(argc, argv, cameraScene());
}
