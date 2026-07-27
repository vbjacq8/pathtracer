#include "box_aabb_impl.h"
#include "constants.h"
#include "hitable.h"
#include "hitable_list.h"
#include "metal.h"
#include "quad.h"
#include "ray.h"
#include "sphere.h"
#include "vec3.h"

#include <gtest/gtest.h>

#include <cmath>
#include <memory>

namespace {

/** Matches pathTrace's tMin floor in color.h (shadow-acne bias). */
constexpr double kHitEps = 0.001;
constexpr double kGeomEps = 1e-6;

MaterialPtr dummyMat() {
    return std::make_shared<Metal>(vec3(0.8, 0.8, 0.8), 0.0f);
}

HitablePtr unitSphereAtOrigin() {
    return std::make_shared<Sphere>(vec3(0, 0, 0), 1.0, dummyMat());
}

}  // namespace

TEST(HitableHit, SphereHitsAtKnownDistance) {
    HitablePtr sphere = unitSphereAtOrigin();
    // Ray from (-2,0,0) toward +x; unit sphere first hit at x = -1 → t = 1.
    const Ray r(vec3(-2, 0, 0), vec3(1, 0, 0));
    HitRecord hr;

    ASSERT_TRUE(sphere->hit(r, 0.0, infinity, hr));
    EXPECT_NEAR(hr.t, 1.0, kGeomEps);
    EXPECT_NEAR(hr.p.x(), -1.0, kGeomEps);
    EXPECT_NEAR(hr.p.y(), 0.0, kGeomEps);
    EXPECT_NEAR(hr.p.z(), 0.0, kGeomEps);
    EXPECT_NEAR(hr.normal.x(), -1.0, kGeomEps);
}

TEST(HitableHit, SphereMissesWhenRaySkipsObject) {
    HitablePtr sphere = unitSphereAtOrigin();
    const Ray r(vec3(-2, 2, 0), vec3(1, 0, 0));
    HitRecord hr;

    EXPECT_FALSE(sphere->hit(r, 0.0, infinity, hr));
}

TEST(HitableHit, SphereHitPointLiesOnSurfaceWithinFloatingPointTolerance) {
    HitablePtr sphere = unitSphereAtOrigin();
    const Ray r(vec3(3, 4, 5), vec3(-3, -4, -5));
    HitRecord hr;

    ASSERT_TRUE(sphere->hit(r, 0.0, infinity, hr));
    const double radiusError = std::fabs(hr.p.norm() - 1.0);
    // Exact algebra would put p on the sphere; FP leaves a small residual.
    EXPECT_LT(radiusError, 1e-9);
    EXPECT_NEAR(hr.normal.norm(), 1.0, kGeomEps);
}

TEST(HitableHit, SphereRejectsHitCloserThanTMin) {
    HitablePtr sphere = unitSphereAtOrigin();
    const Ray r(vec3(-2, 0, 0), vec3(1, 0, 0));
    HitRecord hr;

    // First surface is at t = 1; requiring tMin > 1 should select the far root (t = 3)
    // or miss if tMax is tight.
    EXPECT_FALSE(sphere->hit(r, 1.5, 2.5, hr));
    ASSERT_TRUE(sphere->hit(r, 1.5, 4.0, hr));
    EXPECT_NEAR(hr.t, 3.0, kGeomEps);
    EXPECT_NEAR(hr.p.x(), 1.0, kGeomEps);
}

TEST(HitableHit, SecondaryRayWithoutEpsCanSelfIntersectDueToFloatingPoint) {
    HitablePtr sphere = unitSphereAtOrigin();
    const Ray primary(vec3(-2, 0, 0), vec3(1, 0, 0));
    HitRecord primaryHit;
    ASSERT_TRUE(sphere->hit(primary, 0.0, infinity, primaryHit));

    // Scatter from the recorded hit point back along the surface normal (outward).
    // Algebraically t = 0 is a root; floating-point error can yield a tiny positive t.
    const Ray secondary(primaryHit.p, primaryHit.normal);
    HitRecord secondaryHit;

    if (sphere->hit(secondary, 0.0, infinity, secondaryHit)) {
        // A "hit" this close is a false self-intersection (shadow acne), not geometry ahead.
        EXPECT_LT(secondaryHit.t, kHitEps)
            << "Unexpected distant hit from surface origin; t=" << secondaryHit.t;
    }
    // If FP rounds the near root to exactly 0, strict (tMin,tMax) rejects it — also fine.
}

TEST(HitableHit, SecondaryRayWithHitEpsAvoidsSelfIntersection) {
    HitablePtr sphere = unitSphereAtOrigin();
    const Ray primary(vec3(-2, 0, 0), vec3(1, 0, 0));
    HitRecord primaryHit;
    ASSERT_TRUE(sphere->hit(primary, 0.0, infinity, primaryHit));

    // Same setup as above, but with the path tracer's tMin bias.
    const Ray secondary(primaryHit.p, primaryHit.normal);
    HitRecord secondaryHit;

    EXPECT_FALSE(sphere->hit(secondary, kHitEps, infinity, secondaryHit))
        << "kHitEps should reject near-zero self-hits; got t=" << secondaryHit.t;

    // Outward ray should not hit again; inward ray should hit the far side past the eps.
    const Ray inward(primaryHit.p, -primaryHit.normal);
    HitRecord farHit;
    ASSERT_TRUE(sphere->hit(inward, kHitEps, infinity, farHit));
    EXPECT_GT(farHit.t, kHitEps);
    EXPECT_NEAR(farHit.t, 2.0, 1e-4);
}

TEST(HitableHit, BoxHitRespectsTMinBias) {
    HitablePtr box = std::make_shared<BoxAABBImpl>(vec3(-1, -1, -1), vec3(1, 1, 1), dummyMat());
    const Ray primary(vec3(-3, 0, 0), vec3(1, 0, 0));
    HitRecord primaryHit;
    ASSERT_TRUE(box->hit(primary, 0.0, infinity, primaryHit));
    EXPECT_NEAR(primaryHit.t, 2.0, kGeomEps);

    const Ray secondary(primaryHit.p, primaryHit.normal);
    HitRecord secondaryHit;
    EXPECT_FALSE(box->hit(secondary, kHitEps, infinity, secondaryHit));
}

TEST(HitableHit, HitableListUsesPolymorphicHitThroughHitablePtr) {
    auto world = std::make_shared<HitableList>();
    world->add(unitSphereAtOrigin());
    world->add(std::make_shared<BoxAABBImpl>(vec3(5, -0.5, -0.5), vec3(6, 0.5, 0.5), dummyMat()));

    HitablePtr scene = world;
    HitRecord hr;

    // Hits the nearer sphere first.
    ASSERT_TRUE(scene->hit(Ray(vec3(-5, 0, 0), vec3(1, 0, 0)), kHitEps, infinity, hr));
    EXPECT_NEAR(hr.t, 4.0, kGeomEps);
    EXPECT_NEAR(hr.p.x(), -1.0, kGeomEps);

    // Starts past the sphere so only the box is ahead.
    ASSERT_TRUE(scene->hit(Ray(vec3(2, 0, 0), vec3(1, 0, 0)), kHitEps, infinity, hr));
    EXPECT_NEAR(hr.p.x(), 5.0, kGeomEps);
}

TEST(HitableHit, QuadIsVisibleFromBothSidesOfThePlane) {
    // Unit square in the xz plane at y = 0; cross(u,v) gives geometric normal -y.
    HitablePtr quad = std::make_shared<Quad>(
        vec3(0, 0, 0),
        vec3(1, 0, 0),
        vec3(0, 0, 1),
        dummyMat());

    HitRecord hrFromAbove;
    ASSERT_TRUE(quad->hit(Ray(vec3(0.5, 1, 0.5), vec3(0, -1, 0)), kHitEps, infinity, hrFromAbove));

    HitRecord hrFromBelow;
    ASSERT_TRUE(quad->hit(Ray(vec3(0.5, -1, 0.5), vec3(0, 1, 0)), kHitEps, infinity, hrFromBelow));

    // Both sides register a hit; shading normal always points toward the ray.
    EXPECT_LT(dot(vec3(0, -1, 0), hrFromAbove.normal), 0.0);
    EXPECT_LT(dot(vec3(0, 1, 0), hrFromBelow.normal), 0.0);
}
