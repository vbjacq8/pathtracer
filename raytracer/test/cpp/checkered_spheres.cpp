#include "../../src/materials.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"
#include "../../src/bvh_weekend.h"
//#include "../../src/bvh_jacco.h"
#include "../../src/mvc/interactive.h"

#include <memory>
#include <vector>

HitablePtr checkeredScene(){
    std::vector<HitablePtr> objects;
    auto checker = std::make_shared<Checkered>(0.32, vec3(0.2,0.3,0.1), vec3(0.9, 0.9, 0.9));
    std::shared_ptr<Lambertian> lamb = make_shared<Lambertian>(checker);

    objects.push_back(std::make_shared<Sphere>(vec3(0,-10,0), 10, lamb));
    objects.push_back(std::make_shared<Sphere>(vec3(0, 10, 0), 10, lamb));

    return make_shared<BVHWeekend>(objects);
}

int main(int argc, char** argv){
    return interactiveRender(argc, argv, checkeredScene());

}