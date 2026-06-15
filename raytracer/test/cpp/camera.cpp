#include "../../src/camera.h"
#include "../../src/materials.h"
#include "../../src/renderer.h"


using namespace std;

int main(){
    int nx = 200;
    int ny = 100;
    vec3 lookfrom(3,3,-2);
    vec3 lookat(0,0,-1);
    double distToFocus = (lookat-lookfrom).norm();
    double aperture = 0.1;
    Camera cam(lookfrom, lookat, vec3(0,1,0), 20, double(nx)/double(ny), aperture, distToFocus);
    
    Hitable** list = new Hitable*[2];
    list[0] = new Sphere(vec3(0,0,0), 1, new Metal(vec3(0.9,0.9,0.9), 0.1));
    list[1] = new Sphere(vec3(0,1,0), 1, new Lambertian(vec3(0.5,0.5,0.5)));
    Hitable* world = new HitableList(list, 2);
    Framebuffer fb(nx, ny);
    const int totalSamples = 5;
    render(cam, world, fb, totalSamples, 5, metalColor);
    writePpm(fb, cout);

    return 0;
}
