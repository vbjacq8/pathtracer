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

    // AU-ish coords scaled up so spheres clear the sun and orbits read as separate.
    constexpr float kOrbitScale = 50.0;

    auto earthTexture  = std::make_shared<Wallpaper>("earthmap.jpg");
    auto earthSurface = std::make_shared<Lambertian>(earthTexture);
    auto earth = std::make_shared<Sphere>(
        vec3(-0.20, 0.0, 0.98) * kOrbitScale, 2, earthSurface);

    auto venusTexture  = std::make_shared<Wallpaper>("venusmap.jpeg");
    auto venusSurface = std::make_shared<Lambertian>(venusTexture);
    auto venus = std::make_shared<Sphere>(
        vec3(-0.71, 0.0, -0.14) * kOrbitScale, 2, venusSurface);

    auto mercuryTexture  = std::make_shared<Wallpaper>("mercurymap.jpg");
    auto mercurySurface = std::make_shared<Lambertian>(mercuryTexture);
    auto mercury = std::make_shared<Sphere>(
        vec3(-0.13, 0.0, 0.29) * kOrbitScale, 2, mercurySurface);

    auto marsTexture  = std::make_shared<Wallpaper>("marsmap.jpeg");
    auto marsSurface = std::make_shared<Lambertian>(marsTexture);
    auto mars = std::make_shared<Sphere>(
        vec3(-1.63, 0.0, -0.63) * kOrbitScale, 2, marsSurface);

    auto jupiterTexture  = std::make_shared<Wallpaper>("jupitermap.jpg");
    auto jupiterSurface = std::make_shared<Lambertian>(jupiterTexture);
    auto jupiter = std::make_shared<Sphere>(
        vec3(4.95, 0.0, 1.82) * kOrbitScale, 2, jupiterSurface);

    auto saturnTexture  = std::make_shared<Wallpaper>("saturnmap.jpg");
    auto saturnSurface = std::make_shared<Lambertian>(saturnTexture);
    auto saturn = std::make_shared<Sphere>(
        vec3(9.53, 0.0, -0.85) * kOrbitScale, 2, saturnSurface);

    auto uranusTexture  = std::make_shared<Wallpaper>("uranusmap.jpeg");
    auto uranusSurface = std::make_shared<Lambertian>(uranusTexture);
    auto uranus = std::make_shared<Sphere>(
        vec3(11.93, 0.0, 15.82) * kOrbitScale, 1, uranusSurface);

    auto neptuneTexture  = std::make_shared<Wallpaper>("neptunemap.jpeg");
    auto neptuneSurface = std::make_shared<Lambertian>(neptuneTexture);
    auto neptune = std::make_shared<Sphere>(
        vec3(29.75, 0.0, -1.64) * kOrbitScale, 2, neptuneSurface);

    // High emission: without next-event estimation, light only arrives when a bounce
    // hits the sun; outer planets see a tiny solid angle so need a bright source.
    auto sunlight = std::make_shared<Light>(vec3(2e3, 2e3, 2e3));
    auto sun = make_shared<Sphere>(vec3(0, 0, 0), 6, sunlight);

    objects.push_back(earth);
    objects.push_back(venus);
    objects.push_back(mercury);
    objects.push_back(mars);
    objects.push_back(jupiter);
    objects.push_back(saturn);
    objects.push_back(uranus);
    objects.push_back(neptune);
    objects.push_back(sun);


    //TODO: background stars as mini lights. 

    return std::make_shared<BVHWeekend>(std::move(objects));

}


int main(int argc, char** argv){
    return interactiveRender(argc, argv, globeScene());
}
