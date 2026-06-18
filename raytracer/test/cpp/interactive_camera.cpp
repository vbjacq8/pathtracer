#include "../../src/materials.h"
#include "../../src/mvc/interactive.h"

/**
 * Small two-sphere scene for quickly exercising the SDL viewer.
 * Build and run from the repo root:
 *   ./run_interactive.sh interactive_camera.cpp --width 640 --height 360
 */
Hitable* cameraScene() {
    Hitable** list = new Hitable*[2];
    list[0] = new Sphere(vec3(0, 0, 0), 1, new Metal(vec3(0.9, 0.9, 0.9), 0.1));
    list[1] = new Sphere(vec3(0, 1, 0), 1, new Lambertian(vec3(0.5, 0.5, 0.5)));
    return new HitableList(list, 2);
}

int main(int argc, char** argv) {
    return interactiveRender(argc, argv, cameraScene());
}
