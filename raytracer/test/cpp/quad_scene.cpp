#include "../../src/materials.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"
#include "../../src/bvh_weekend.h"
//#include "../../src/bvh_jacco.h"
#include "../../src/mvc/interactive.h"

#include <memory>
#include <vector>

HitablePtr quadScene(){
    std::vector<HitablePtr> objects;

    // Materials
    auto left_red     = std::make_shared<Lambertian>(vec3(1.0, 0.2, 0.2));
    auto back_green   = std::make_shared<Lambertian>(vec3(0.2, 1.0, 0.2));
    auto right_blue   = std::make_shared<Lambertian>(vec3(0.2, 0.2, 1.0));
    auto upper_orange = std::make_shared<Lambertian>(vec3(1.0, 0.5, 0.0));
    auto lower_teal   = std::make_shared<Lambertian>(vec3(0.2, 0.8, 0.8));

    // Quads
    objects.push_back(make_shared<Quad>(vec3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    objects.push_back(make_shared<Quad>(vec3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    objects.push_back(make_shared<Quad>(vec3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    objects.push_back(make_shared<Quad>(vec3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    objects.push_back(make_shared<Quad>(vec3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    return make_shared<BVHWeekend>(std::move(objects));
}



int main(int argc, char** argv){
    return interactiveRender(argc, argv, quadScene());
}