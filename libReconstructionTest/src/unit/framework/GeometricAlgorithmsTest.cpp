#include "stdafx.h"
#include "framework/error/Exception.h"
#include "framework/error/GeometricComputationException.h"
#include "framework/geometry/GeometricAlgorithms.h"
#include "framework/geometry/Ray.h"
#include "framework/geometry/Visualizer.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"
#include "framework/math/Vec4.h"
#include "framework/test/TestBase.h"

namespace ettention
{
    class GeometricAlgorithmsTest : public TestBase {};
}

using namespace ettention;

TEST_F(GeometricAlgorithmsTest, ClosestPointOfTwoRays_Case_A)
{
    Vec3f pointA(-1.0f,  0.0f, 0.0f);
    Vec3f dirA(1.0f,  0.0f, 0.0f);

    Vec3f pointB(0.0f, -1.0f, 0.0f);
    Vec3f dirB(0.0f,  1.0f, 0.0f);

    Ray a = Ray::FromPointAndDirection(pointA, dirA);
    Ray b = Ray::FromPointAndDirection(pointB, dirB);

    auto result = GeometricAlgorithms::GetClosestPointsOfRays(a, b);
    ASSERT_FLOAT_EQ(result.first.x, 0.0f);
    ASSERT_FLOAT_EQ(result.first.y, 0.0f);
    ASSERT_FLOAT_EQ(result.first.z, 0.0f);
    ASSERT_FLOAT_EQ(result.second.x, 0.0f);
    ASSERT_FLOAT_EQ(result.second.y, 0.0f);
    ASSERT_FLOAT_EQ(result.second.z, 0.0f);
}

TEST_F(GeometricAlgorithmsTest, ClosestPointOfTwoRays_Case_B)
{
    Vec3f pointA(-1.0f,  0.0f, 0.0f);
    Vec3f dirA(1.0f,  0.5f, 0.0f);

    Vec3f pointB(0.0f, -1.0f, 0.0f);
    Vec3f dirB(0.5f,  1.0f, 0.0f);

    Ray a = Ray::FromPointAndDirection(pointA, dirA);
    Ray b = Ray::FromPointAndDirection(pointB, dirB);

    auto result = GeometricAlgorithms::GetClosestPointsOfRays(a, b);
    ASSERT_FLOAT_EQ(result.first.x, 1.0f);
    ASSERT_FLOAT_EQ(result.first.y, 1.0f);
    ASSERT_FLOAT_EQ(result.first.z, 0.0f);
    ASSERT_FLOAT_EQ(result.second.x, 1.0f);
    ASSERT_FLOAT_EQ(result.second.y, 1.0f);
    ASSERT_FLOAT_EQ(result.second.z, 0.0f);
}

TEST_F(GeometricAlgorithmsTest, ClosestPointOfTwoRays_Case_Parallel)
{
    Vec3f pointA(-1.0f, 0.0f, 0.0f);
    Vec3f pointB(0.0f, -1.0f, 0.0f);
    Vec3f dir(1.0f, 0.5f, 0.0f);

    Ray a = Ray::FromPointAndDirection(pointA, dir);
    Ray b = Ray::FromPointAndDirection(pointB, dir);

    ASSERT_THROW(GeometricAlgorithms::GetClosestPointsOfRays(a, b), GeometricComputationException);
}

TEST_F(GeometricAlgorithmsTest, ClipRayAgainstBoundingBox_Case_Parallel)
{
    Vec3f pointA(0.5f, 0.5f, 0.5f);
    Vec3f dir(1.0f, 0.0f, 0.0f);

    Ray ray = Ray::FromPointAndDirection(pointA, dir);
    BoundingBox3f aabb(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 1.0f));

    auto tLimits = GeometricAlgorithms::ClipRayAgainstBoundingBox(ray, aabb);
    ASSERT_FLOAT_EQ(tLimits.first, -0.5f);
    ASSERT_FLOAT_EQ(tLimits.second, 0.5f);
}

TEST_F(GeometricAlgorithmsTest, ClipRayAgainstBoundingBox_Case_Diagonal)
{
    Vec3f pointA(0.5f, 0.5f, 0.5f);
    Vec3f dir(1.0f, 1.0f, 0.0f);

    Ray ray = Ray::FromPointAndDirection(pointA, dir);
    BoundingBox3f aabb(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 1.0f, 1.0f));

    auto tLimits = GeometricAlgorithms::ClipRayAgainstBoundingBox(ray, aabb);
    ASSERT_FLOAT_EQ(tLimits.first,  -sqrtf(2.0f) / 2.0f);
    ASSERT_FLOAT_EQ(tLimits.second,  sqrtf(2.0f) / 2.0f);
}
