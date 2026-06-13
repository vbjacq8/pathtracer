#include "../../src/camera.h"
#include "../../src/materials.h"


using namespace std;

int main(){
    int nx = 200;
    int ny = 100;
    cout << "P3\n" << nx << " " << ny << "\n255\n";
    vec3 lookfrom(3,3,-2);
    vec3 lookat(0,0,-1);
    double distToFocus = (lookat-lookfrom).norm();
    double aperture = 2.0;
    Camera cam(lookfrom, lookat, vec3(0,1,0), 20, double(nx)/double(ny), aperture, distToFocus);
    
    Hitable** list = new Hitable*[2];
    list[0] = new Sphere(vec3(0,0,0), 1, new Metal(vec3(0.9,0.9,0.9), 1));
    list[1] = new Sphere(vec3(0,1,0), 1, new Lambertian(vec3(0.5,0.5,0.5)));
    Hitable* world = new HitableList(list, 2);


    for (int j = ny-1; j >=0 ; j--){
        for (int i = 0; i < nx; i++){
            vec3 col = cam.colorSample(i,j,nx,ny, 5, world, 5, metalColor);
            int ir = (int) (255.99 * col[0]);
            int ib = (int) (255.99 * col[1]);
            int ig = (int) (255.99 * col[2]);
            cout << ir << " " << ig << " " << ib << endl;

        }
    }
    return 0;



}