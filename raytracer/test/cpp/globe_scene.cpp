#include "../../src/materials.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"
#include "../../src/bvh_weekend.h"
//#include "../../src/bvh_jacco.h"
#include "../../src/mvc/interactive.h"

#include <memory>
#include <vector>

HitablePtr globeScene(){
    std::vector<HitablePtr> objects;
    
    auto earthTexture  = new Wallpaper("earthmap.jpg");
    auto earthSurface = new Lambertian(earthTexture);
    auto earth = std::make_shared<Sphere>(vec3(0,0,0), 1, earthSurface);

    auto grassTexture  = new Wallpaper("grass.jpg");
    auto grassSurface = new Lambertian(grassTexture);
    auto grass  = std::make_shared<Sphere>(vec3(2.1,0,0), 1, grassSurface);

    auto cobbleTexture = new Wallpaper("cobble.png");
    auto cobbleSurface = new Lambertian(cobbleTexture);
    auto cobble = std::make_shared<Sphere>(vec3(-2.1, 0, 0), 1, cobbleSurface);

    auto cosmicTexture = new Wallpaper("cosmic.jpeg");
    auto cosmicSurface = new Lambertian(cosmicTexture);
    auto cosmic = std::make_shared<Sphere>(vec3(0, 0, -2.1), 1, cosmicSurface);

    auto sunlight = new Light(vec3(255, 255, 255));
    auto sun = std::make_shared<Sphere>(vec3(0,1,4.1), 2, sunlight);

    objects.push_back(earth);
    objects.push_back(cobble);
    objects.push_back(grass);
    objects.push_back(cosmic);
    objects.push_back(sun);
    return std::make_shared<BVHWeekend>(std::move(objects));

}


int main(int argc, char** argv){
    return interactiveRender(argc, argv, globeScene(), colorVoid);
}