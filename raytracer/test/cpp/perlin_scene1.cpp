#include "../../src/materials.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"
#include "../../src/bvh_weekend.h"
//#include "../../src/bvh_jacco.h"
#include "../../src/mvc/interactive.h"

#include <memory>
#include <vector>


HitablePtr perlinSceneOne(){
    std::vector<HitablePtr> objects;

    auto* perlMat = new Lambertian(new Noise(4));
    objects.push_back(std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, perlMat));
    objects.push_back(std::make_shared<Sphere>(vec3(0, 2, 0), 2, perlMat));


    return std::make_shared<BVHWeekend>(std::move(objects));

}

int main (int argc, char** argv){
    return interactiveRender(argc,argv, perlinSceneOne());
}