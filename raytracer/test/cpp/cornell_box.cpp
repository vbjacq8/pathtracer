#include "../../src/materials.h"
#include "../../src/hittables.h"
#include "../../src/my_random.h"
#include "../../src/bvh_weekend.h"
//#include "../../src/bvh_jacco.h"
#include "../../src/mvc/interactive.h"

#include <memory>
#include <vector>

/** 
 * \brief Cornell Box scene; reccomended CLI arguments: --lookfrom 278 278 -800 --lookat 278 278 0 --vfov 40

*/

HitablePtr cornellBox(){
    std::vector<HitablePtr> objects;
    auto red   = new Lambertian(vec3(.65, .05, .05));
    auto white =new Lambertian(vec3(.73, .73, .73));
    auto green =new Lambertian(vec3(.12, .45, .15));
    auto light =new DiffuseLight(vec3(15, 15, 15));

    objects.push_back(std::make_shared<Quad>(vec3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    objects.push_back(std::make_shared<Quad>(vec3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    objects.push_back(std::make_shared<Quad>(vec3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    objects.push_back(std::make_shared<Quad>(vec3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    objects.push_back(std::make_shared<Quad>(vec3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    objects.push_back(std::make_shared<Quad>(vec3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    std::shared_ptr<Hitable> box1 = std::make_shared<BoxQuadImpl>(vec3(0,0,0), vec3(165, 330, 165), white);
    std::shared_ptr<Hitable> box2 = std::make_shared<BoxQuadImpl>(vec3(0,0,0), vec3(165,165, 165), white);

    box1 = std::make_shared<RotateY>(box1, 15);
    box2 = std::make_shared<RotateY>(box2, -18);

    box1 = std::make_shared<Translate>(box1, vec3(265, 0, 295));
    box2 = std::make_shared<Translate>(box2, vec3(130,0,65));

    objects.push_back(box1);
    objects.push_back(box2);

    return std::make_shared<BVHWeekend>(std::move(objects));
}

int main (int argc, char** argv){
    return interactiveRender(argc,argv, cornellBox(), colorVoid);
}

