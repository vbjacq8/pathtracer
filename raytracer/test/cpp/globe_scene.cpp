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
    
    auto earthTexture  = std::make_shared<Wallpaper>("earthmap.jpg");
    auto earthSurface = std::make_shared<Lambertian>(earthTexture);
    auto earth = std::make_shared<Sphere>(vec3(0,0,0), 1, earthSurface);

    auto grassTexture  = std::make_shared<Wallpaper>("grass.jpg");
    auto grassSurface = std::make_shared<Lambertian>(grassTexture);
    auto grass  = std::make_shared<Sphere>(vec3(2.1,0,0), 1, grassSurface);

    auto cobbleTexture = std::make_shared<Wallpaper>("cobble.png");
    auto cobbleSurface = make_shared<Lambertian>(cobbleTexture);
    auto cobble = make_shared<Sphere>(vec3(-2.1, 0, 0), 1, cobbleSurface);

    auto cosmicTexture = std::make_shared<Wallpaper>("cosmic.jpeg");
    auto cosmicSurface = make_shared<Lambertian>(cosmicTexture);
    auto cosmic = make_shared<Sphere>(vec3(0, 0, -2.1), 1, cosmicSurface);

    auto sunlight = std::make_shared<Light>(vec3(255, 255, 255));
    auto sun = make_shared<Sphere>(vec3(0,1,4.1), 2, sunlight);

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