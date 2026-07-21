#include "hittables.h"
#include "my_random.h"
#include "texture.h"

class DiffuseLight : public Material {
    public: 
        DiffuseLight(std::shared_ptr<Texture> tex) : tex(tex) {}
        DiffuseLight(const vec3& emit) : tex(std::make_shared<SolidColor>(emit)) {}

        vec3 emit(double u, double v, const vec3& p) const override {
            return tex->value(u, v, p);
        }

        bool scatter(const Ray&, const HitRecord&, vec3&, Ray&) const override {
            return false;
        }
        
    private:
        std::shared_ptr<Texture> tex;
};