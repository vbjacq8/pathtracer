#include "../../src/materials.h"
#include "../../src/mvc/interactive.h"
#include "../../src/hittables.h"

#include <memory>

/**
 * Small two-sphere scene for quickly exercising the SDL viewer.
 * Build and run from the repo root:
 *   ./run_interactive.sh interactive_camera.cpp --width 640 --height 360
 */
HitablePtr cameraScene() {
    auto world = std::make_shared<HitableList>();
    world->add(std::make_shared<Sphere>(
        vec3(0, 0, 0), 1, std::make_shared<Metal>(vec3(0.9, 0.9, 0.9), 0.1)));
    world->add(std::make_shared<Sphere>(
        vec3(0, 1, 0),
        1,
        std::make_shared<Lambertian>(std::make_shared<SolidColor>(vec3(0.5, 0.5, 0.5)))));
    return world;
}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, cameraScene());
}
