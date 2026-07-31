#include "vec3.h"

#include <gtest/gtest.h>

#include <cmath>

namespace {

constexpr float kEps = 1e-5f;

}  // namespace

TEST(Vec3, DefaultConstructLeavesUnspecifiedComponents) {
    // Default ctor does not zero-initialize; only check that construction succeeds.
    vec3 v;
    (void)v;
    SUCCEED();
}

TEST(Vec3, ComponentAccessors) {
    const vec3 v(1.5, -2.0, 3.25);
    EXPECT_FLOAT_EQ(v.x(), 1.5);
    EXPECT_FLOAT_EQ(v.y(), -2.0);
    EXPECT_FLOAT_EQ(v.z(), 3.25);
    EXPECT_FLOAT_EQ(v.r(), 1.5);
    EXPECT_FLOAT_EQ(v.g(), -2.0);
    EXPECT_FLOAT_EQ(v.b(), 3.25);
    EXPECT_FLOAT_EQ(v[0], 1.5);
    EXPECT_FLOAT_EQ(v[1], -2.0);
    EXPECT_FLOAT_EQ(v[2], 3.25);
}

TEST(Vec3, UnaryMinus) {
    vec3 v(1.0, -2.0, 3.0);
    const vec3 n = -v;
    EXPECT_FLOAT_EQ(n.x(), -1.0);
    EXPECT_FLOAT_EQ(n.y(), 2.0);
    EXPECT_FLOAT_EQ(n.z(), -3.0);
}

TEST(Vec3, AdditionAndSubtraction) {
    const vec3 a(1.0, 2.0, 3.0);
    const vec3 b(4.0, 5.0, 6.0);
    const vec3 sum = a + b;
    const vec3 diff = b - a;

    EXPECT_FLOAT_EQ(sum.x(), 5.0);
    EXPECT_FLOAT_EQ(sum.y(), 7.0);
    EXPECT_FLOAT_EQ(sum.z(), 9.0);
    EXPECT_FLOAT_EQ(diff.x(), 3.0);
    EXPECT_FLOAT_EQ(diff.y(), 3.0);
    EXPECT_FLOAT_EQ(diff.z(), 3.0);
}

TEST(Vec3, ElementWiseAndScalarMultiply) {
    const vec3 a(2.0, 3.0, 4.0);
    const vec3 b(5.0, 6.0, 7.0);
    const vec3 hadamard = a * b;
    const vec3 scaled = a * 2.0;
    const vec3 scaledLeft = 3.0 * a;

    EXPECT_FLOAT_EQ(hadamard.x(), 10.0);
    EXPECT_FLOAT_EQ(hadamard.y(), 18.0);
    EXPECT_FLOAT_EQ(hadamard.z(), 28.0);
    EXPECT_FLOAT_EQ(scaled.x(), 4.0);
    EXPECT_FLOAT_EQ(scaled.y(), 6.0);
    EXPECT_FLOAT_EQ(scaled.z(), 8.0);
    EXPECT_FLOAT_EQ(scaledLeft.x(), 6.0);
    EXPECT_FLOAT_EQ(scaledLeft.y(), 9.0);
    EXPECT_FLOAT_EQ(scaledLeft.z(), 12.0);
}

TEST(Vec3, Division) {
    const vec3 a(2.0, 4.0, 8.0);
    const vec3 b(2.0, 2.0, 2.0);
    const vec3 elementWise = a / b;
    const vec3 scaled = a / 2.0;

    EXPECT_FLOAT_EQ(elementWise.x(), 1.0);
    EXPECT_FLOAT_EQ(elementWise.y(), 2.0);
    EXPECT_FLOAT_EQ(elementWise.z(), 4.0);
    EXPECT_FLOAT_EQ(scaled.x(), 1.0);
    EXPECT_FLOAT_EQ(scaled.y(), 2.0);
    EXPECT_FLOAT_EQ(scaled.z(), 4.0);
}

TEST(Vec3, CompoundAssignment) {
    vec3 v(1.0, 2.0, 3.0);
    v += vec3(1.0, 1.0, 1.0);
    EXPECT_FLOAT_EQ(v.x(), 2.0);
    EXPECT_FLOAT_EQ(v.y(), 3.0);
    EXPECT_FLOAT_EQ(v.z(), 4.0);

    v -= vec3(0.5, 0.5, 0.5);
    EXPECT_FLOAT_EQ(v.x(), 1.5);
    EXPECT_FLOAT_EQ(v.y(), 2.5);
    EXPECT_FLOAT_EQ(v.z(), 3.5);

    v *= 2.0;
    EXPECT_FLOAT_EQ(v.x(), 3.0);
    EXPECT_FLOAT_EQ(v.y(), 5.0);
    EXPECT_FLOAT_EQ(v.z(), 7.0);

    v /= 2.0;
    EXPECT_FLOAT_EQ(v.x(), 1.5);
    EXPECT_FLOAT_EQ(v.y(), 2.5);
    EXPECT_FLOAT_EQ(v.z(), 3.5);
}

TEST(Vec3, NormAndSquaredNorm) {
    const vec3 v(3.0, 4.0, 0.0);
    EXPECT_FLOAT_EQ(v.squared_norm(), 25.0);
    EXPECT_FLOAT_EQ(v.norm(), 5.0);

    const vec3 w(1.0, 2.0, 2.0);
    EXPECT_NEAR(w.norm(), std::sqrt(9.0), kEps);
    EXPECT_FLOAT_EQ(w.squared_norm(), 9.0);
}

TEST(Vec3, DotAndCross) {
    const vec3 a(1.0, 0.0, 0.0);
    const vec3 b(0.0, 1.0, 0.0);
    EXPECT_FLOAT_EQ(dot(a, b), 0.0);
    EXPECT_FLOAT_EQ(dot(a, a), 1.0);

    const vec3 c = cross(a, b);
    EXPECT_NEAR(c.x(), 0.0, kEps);
    EXPECT_NEAR(c.y(), 0.0, kEps);
    EXPECT_NEAR(c.z(), 1.0, kEps);

    // Cross is orthogonal to both inputs.
    EXPECT_NEAR(dot(c, a), 0.0, kEps);
    EXPECT_NEAR(dot(c, b), 0.0, kEps);
}

TEST(Vec3, UnitVectorAndMakeUnit) {
    const vec3 v(3.0, 0.0, 4.0);
    const vec3 u = unit_vector(v);
    EXPECT_NEAR(u.norm(), 1.0, kEps);
    EXPECT_NEAR(u.x(), 0.6, kEps);
    EXPECT_NEAR(u.z(), 0.8, kEps);

    vec3 w(0.0, 5.0, 0.0);
    w.make_unit_vector();
    EXPECT_NEAR(w.norm(), 1.0, kEps);
    EXPECT_NEAR(w.y(), 1.0, kEps);
}

TEST(Vec3, NearZero) {
    EXPECT_TRUE(vec3(0.0, 0.0, 0.0).near_zero());
    EXPECT_TRUE(vec3(1e-9, -1e-9, 0.0).near_zero());
    EXPECT_FALSE(vec3(1e-6, 0.0, 0.0).near_zero());
    EXPECT_FALSE(vec3(0.0, 0.1, 0.0).near_zero());
}
