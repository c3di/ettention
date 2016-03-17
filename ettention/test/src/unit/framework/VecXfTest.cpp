#include "stdafx.h"
#include "framework/error/Exception.h"
#include "framework/math/Vec3.h"
#include "framework/math/Vec4.h"
#include "framework/test/TestBase.h"

namespace ettention
{
    class VecXfTest : public TestBase {};
}

using namespace ettention;

TEST_F(VecXfTest, DefaultConstructorInitializesToZero)
{
    Vec3f vec3;
    ASSERT_FLOAT_EQ(vec3.x, 0.0f);
    ASSERT_FLOAT_EQ(vec3.y, 0.0f);
    ASSERT_FLOAT_EQ(vec3.z, 0.0f);

    Vec4f vec4;
    ASSERT_FLOAT_EQ(vec4.x, 0.0f);
    ASSERT_FLOAT_EQ(vec4.y, 0.0f);
    ASSERT_FLOAT_EQ(vec4.z, 0.0f);
    ASSERT_FLOAT_EQ(vec4.w, 0.0f);
}

TEST_F(VecXfTest, ElementWiseConstructor)
{
    const float anyValue = 1.f;
    const float anyIncrement = 2.f;
    Vec3f vec3(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement);
    ASSERT_FLOAT_EQ(vec3.x, anyValue);
    ASSERT_FLOAT_EQ(vec3.y, anyValue + anyIncrement);
    ASSERT_FLOAT_EQ(vec3.z, anyValue + 2 * anyIncrement);

    Vec4f vec4(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement, anyValue + 3 * anyIncrement);
    ASSERT_FLOAT_EQ(vec4.x, anyValue);
    ASSERT_FLOAT_EQ(vec4.y, anyValue + anyIncrement);
    ASSERT_FLOAT_EQ(vec4.z, anyValue + 2 * anyIncrement);
    ASSERT_FLOAT_EQ(vec4.w, anyValue + 3 * anyIncrement);
}

TEST_F(VecXfTest, CopyConstructor)
{
    const float anyValue = 1.f;
    const float anyIncrement = 2.f;
    Vec3f vec3(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement);
    Vec3f vec3Other = vec3;
    ASSERT_FLOAT_EQ(vec3.x, vec3Other.x);
    ASSERT_FLOAT_EQ(vec3.y, vec3Other.y);
    ASSERT_FLOAT_EQ(vec3.z, vec3Other.z);


    Vec4f vec4(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement, anyValue + 3 * anyIncrement);
    Vec4f vec4Other = vec4;
    ASSERT_FLOAT_EQ(vec4.x, vec4Other.x);
    ASSERT_FLOAT_EQ(vec4.y, vec4Other.y);
    ASSERT_FLOAT_EQ(vec4.z, vec4Other.z);
    ASSERT_FLOAT_EQ(vec4.w, vec4Other.w);
}

TEST_F(VecXfTest, AssignmentOperator)
{
    const float anyValue = 1.f;
    const float anyIncrement = 2.f;
    Vec3f vec3(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement);
    Vec3f vec3result;
    vec3result = vec3;
    ASSERT_FLOAT_EQ(vec3result.x, vec3.x);
    ASSERT_FLOAT_EQ(vec3result.y, vec3.y);
    ASSERT_FLOAT_EQ(vec3result.z, vec3.z);


    Vec4f vec4(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement, anyValue + 3 * anyIncrement);
    Vec4f vec4result = vec4;
    ASSERT_FLOAT_EQ(vec4result.x, vec4.x);
    ASSERT_FLOAT_EQ(vec4result.y, vec4.y);
    ASSERT_FLOAT_EQ(vec4result.z, vec4.z);
    ASSERT_FLOAT_EQ(vec4result.w, vec4.w);
}

TEST_F(VecXfTest, PlusOperator)
{
    const float anyValue = 1.0f;
    const float anyIncrement = 2.0f;
    Vec3f vec3(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement);
    Vec3f vec3Other(anyValue, anyValue, anyValue);
    Vec3f vec3result;
    vec3result = vec3 + vec3Other;
    ASSERT_FLOAT_EQ(vec3result.x, vec3.x + vec3Other.x);
    ASSERT_FLOAT_EQ(vec3result.y, vec3.y + vec3Other.y);
    ASSERT_FLOAT_EQ(vec3result.z, vec3.z + vec3Other.z);


    Vec4f vec4(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement, anyValue + 3 * anyIncrement);
    Vec4f vec4Other(anyValue, anyValue, anyValue, anyValue);
    Vec4f vec4result = vec4 + vec4Other;
    ASSERT_FLOAT_EQ(vec4result.x, vec4.x + vec4Other.x);
    ASSERT_FLOAT_EQ(vec4result.y, vec4.y + vec4Other.y);
    ASSERT_FLOAT_EQ(vec4result.z, vec4.z + vec4Other.z);
    ASSERT_FLOAT_EQ(vec4result.w, vec4.w + vec4Other.w);
}

TEST_F(VecXfTest, MinusOperator)
{
    const float anyValue = 1.f;
    const float anyIncrement = 2.f;
    Vec3f vec3(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement);
    Vec3f vec3Other(anyValue, anyValue, anyValue);
    Vec3f vec3result;
    vec3result = vec3 - vec3Other;
    ASSERT_FLOAT_EQ(vec3result.x, vec3.x - vec3Other.x);
    ASSERT_FLOAT_EQ(vec3result.y, vec3.y - vec3Other.y);
    ASSERT_FLOAT_EQ(vec3result.z, vec3.z - vec3Other.z);


    Vec4f vec4(anyValue, anyValue + anyIncrement, anyValue + 2 * anyIncrement, anyValue + 3 * anyIncrement);
    Vec4f vec4Other(anyValue, anyValue, anyValue, anyValue);
    Vec4f vec4result = vec4 - vec4Other;
    ASSERT_FLOAT_EQ(vec4result.x, vec4.x - vec4Other.x);
    ASSERT_FLOAT_EQ(vec4result.y, vec4.y - vec4Other.y);
    ASSERT_FLOAT_EQ(vec4result.z, vec4.z - vec4Other.z);
    ASSERT_FLOAT_EQ(vec4result.w, vec4.w - vec4Other.w);
}

TEST_F(VecXfTest, PlaneCreation)
{
    Vec3f p0(0.0f, 0.0f, 0.0f);
    Vec3f p1(1.0f, 0.0f, 0.0f);
    Vec3f p2(0.0f, 0.0f, 1.0f);
    //Vec4f plane = planeFromPoints(p0, p1, p2);
    //ASSERT_FLOAT_EQ(Plane.)
}