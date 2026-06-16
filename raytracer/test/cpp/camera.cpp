#include "../../src/camera.h"
#include "../../src/materials.h"
#include "../../src/renderer.h"
#include "../../src/render.h"


using namespace std;


    Hitable* cameraScene(){
        Hitable** list = new Hitable*[2];
        list[0] = new Sphere(vec3(0,0,0), 1, new Metal(vec3(0.9,0.9,0.9), 0.1));
        list[1] = new Sphere(vec3(0,1,0), 1, new Lambertian(vec3(0.5,0.5,0.5)));
        Hitable* world = new HitableList(list, 2);
        return world;

    }

int main(int argc, char** argv){
    render(argc, argv, cameraScene());
    return 0;
}
