#include "stdafx.h"
#include "framework/error/Exception.h"
#include "framework/math/Matrix4x4.h"
#include "framework/math/Vec4.h"
#include "framework/test/TestBase.h"

namespace ettention
{
    class Matrix4x4Test : public TestBase {};
}

using namespace ettention;

void assertNear(const Vec3f& left, const Vec3f& right, float epsilon)
{
    ASSERT_NEAR(left.x, right.x, epsilon);
    ASSERT_NEAR(left.y, right.y, epsilon);
    ASSERT_NEAR(left.z, right.z, epsilon);
}

void assertNear(const Matrix4x4& left, const Matrix4x4& right, float epsilon)
{
    ASSERT_NEAR(left.at(0, 0), right.at(0, 0), epsilon);
    ASSERT_NEAR(left.at(0, 1), right.at(0, 1), epsilon);
    ASSERT_NEAR(left.at(0, 2), right.at(0, 2), epsilon);
    ASSERT_NEAR(left.at(0, 3), right.at(0, 3), epsilon);
    ASSERT_NEAR(left.at(1, 0), right.at(1, 0), epsilon);
    ASSERT_NEAR(left.at(1, 1), right.at(1, 1), epsilon);
    ASSERT_NEAR(left.at(1, 2), right.at(1, 2), epsilon);
    ASSERT_NEAR(left.at(1, 3), right.at(1, 3), epsilon);
    ASSERT_NEAR(left.at(2, 0), right.at(2, 0), epsilon);
    ASSERT_NEAR(left.at(2, 1), right.at(2, 1), epsilon);
    ASSERT_NEAR(left.at(2, 2), right.at(2, 2), epsilon);
    ASSERT_NEAR(left.at(2, 3), right.at(2, 3), epsilon);
    ASSERT_NEAR(left.at(3, 0), right.at(3, 0), epsilon);
    ASSERT_NEAR(left.at(3, 1), right.at(3, 1), epsilon);
    ASSERT_NEAR(left.at(3, 2), right.at(3, 2), epsilon);
    ASSERT_NEAR(left.at(3, 3), right.at(3, 3), epsilon);
}

TEST_F(Matrix4x4Test, UnitIdentity)
{
    Matrix4x4 m = Matrix4x4::identity;
    ASSERT_FLOAT_EQ(m.at(0,0), 1.0f);
    ASSERT_FLOAT_EQ(m.at(0,1), 0.0f);
    ASSERT_FLOAT_EQ(m.at(0,2), 0.0f);
    ASSERT_FLOAT_EQ(m.at(0,3), 0.0f);
    ASSERT_FLOAT_EQ(m.at(1,0), 0.0f);
    ASSERT_FLOAT_EQ(m.at(1,1), 1.0f);
    ASSERT_FLOAT_EQ(m.at(1,2), 0.0f);
    ASSERT_FLOAT_EQ(m.at(1,3), 0.0f);
    ASSERT_FLOAT_EQ(m.at(2,0), 0.0f);
    ASSERT_FLOAT_EQ(m.at(2,1), 0.0f);
    ASSERT_FLOAT_EQ(m.at(2,2), 1.0f);
    ASSERT_FLOAT_EQ(m.at(2,3), 0.0f);
    ASSERT_FLOAT_EQ(m.at(3,0), 0.0f);
    ASSERT_FLOAT_EQ(m.at(3,1), 0.0f);
    ASSERT_FLOAT_EQ(m.at(3,2), 0.0f);
    ASSERT_FLOAT_EQ(m.at(3,3), 1.0f);
}

TEST_F(Matrix4x4Test, UnitConstructor2)
{
    Matrix4x4 m = Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    ASSERT_TRUE (m == Matrix4x4::identity);
}

TEST_F(Matrix4x4Test, UnitConstructor3)
{
    std::vector<float> v;
    v.resize(16);
    v[0] = 1.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
    v[3] = 0.0f;

    v[4] = 0.0f;
    v[5] = 1.0f;
    v[6] = 0.0f;
    v[7] = 0.0f;

    v[8] = 0.0f;
    v[9] = 0.0f;
    v[10] = 1.0f;
    v[11] = 0.0f;

    v[12] = 0.0f;
    v[13] = 0.0f;
    v[14] = 0.0f;
    v[15] = 1.0f;
    Matrix4x4 m = Matrix4x4(v);
    ASSERT_TRUE (m == Matrix4x4::identity);

    v.resize(15);
    ASSERT_THROW(m = Matrix4x4(v), Exception);

    v.resize(17);
    ASSERT_THROW(m = Matrix4x4(v), Exception);
}


TEST_F(Matrix4x4Test, UnitInvertScaling)
{
    Matrix4x4 m = Matrix4x4::identity;
    Matrix4x4 inverse = m.inverse();
    ASSERT_TRUE (m == inverse);

    m.at(0,0) *= 2.0f;
    m.at(1,1) *= 2.0f;
    m.at(2,2) *= 2.0f;
    m.at(3,3) *= 2.0f;

    inverse = m.inverse();

    ASSERT_FLOAT_EQ(inverse.at(0,0), 0.5f);
    ASSERT_FLOAT_EQ(inverse.at(0,1), 0.0f);
    ASSERT_FLOAT_EQ(inverse.at(0,2), 0.0f);
    ASSERT_FLOAT_EQ(inverse.at(1,0), 0.0f);
    ASSERT_FLOAT_EQ(inverse.at(1,1), 0.5f);
    ASSERT_FLOAT_EQ(inverse.at(1,2), 0.0f);
    ASSERT_FLOAT_EQ(inverse.at(2,0), 0.0f);
    ASSERT_FLOAT_EQ(inverse.at(2,1), 0.0f);
    ASSERT_FLOAT_EQ(inverse.at(2,2), 0.5f);
}

TEST_F(Matrix4x4Test, UnitInvert)
{
    static const float EPS = 1e-6f;
    Matrix4x4 m(+2.0f, +0.5f, +0.0f, -0.5f,
                +1.0f, +0.0f, -1.0f, +0.5f,
                -0.5f, -1.0f, +0.0f, +0.0f,
                -1.0f, -1.0f, -1.0f, +0.0f);
    Matrix4x4 mInv(+ 4.0f / 13.0f, + 4.0f / 13.0f, + 6.0f / 13.0f, - 4.0f / 13.0f,
                   - 2.0f / 13.0f, - 2.0f / 13.0f, -16.0f / 13.0f, + 2.0f / 13.0f,
                   - 2.0f / 13.0f, - 2.0f / 13.0f, +10.0f / 13.0f, -11.0f / 13.0f,
                   -12.0f / 13.0f, +14.0f / 13.0f, + 8.0f / 13.0f, -14.0f / 13.0f);
    auto det = m.determinant();
    ASSERT_NEAR(det, 13.0f / 8.0f, EPS);
    auto product = m * mInv;
    assertNear(product, Matrix4x4::identity, EPS);
    auto inverted = m.inverse();
    assertNear(inverted, mInv, EPS);
    assertNear(inverted * m, Matrix4x4::identity, EPS);
}

TEST_F(Matrix4x4Test, UnitInvert_Translation)
{
    Vec3f translationVector = Vec3f (10.0f, -5.0, 10.0f);
    Matrix4x4 m = Matrix4x4::translationMatrix(translationVector);
    Matrix4x4 inverse = m.inverse();

    Vec3f anyPoint = Vec3f (20.0f, 1.0f, -3.4f);

    Vec3f translated = transformCoord(inverse, anyPoint);

    ASSERT_TRUE (translated == anyPoint - translationVector);
}

TEST_F(Matrix4x4Test, UnitMultiplyVector)
{
    Matrix4x4 m = Matrix4x4::identity;

    m.at(0,0) *= 2.0f;
    m.at(1,1) *= 2.0f;
    m.at(2,2) *= 2.0f;

    Matrix4x4 inverse = m.inverse();

    Vec3f v = Vec3f(1.0f, 2.0f, 3.0f);

    Vec3f vStrich = transformCoord(inverse, v);

    ASSERT_FLOAT_EQ(vStrich.x, 0.5f);
    ASSERT_FLOAT_EQ(vStrich.y, 1.0f);
    ASSERT_FLOAT_EQ(vStrich.z, 1.5f);
}

TEST_F(Matrix4x4Test, UnitMultiplyMatrix)
{
    {
        Matrix4x4 m = Matrix4x4::identity;
        Matrix4x4 n = Matrix4x4::translationMatrix(Vec3f(1.0f, 2.0f, 3.0f));
        Matrix4x4 result = multiply(m, n);
        ASSERT_TRUE(result == n); 
    }

    {
        Matrix4x4 m = Matrix4x4::scaleMatrix(2.0f);
        Matrix4x4 n = Matrix4x4::translationMatrix(Vec3f(1.0f, 2.0f, 3.0f));
        Matrix4x4 result = multiply(m, n);
        ASSERT_FLOAT_EQ(result.at(3,0), 2.0f); 
        ASSERT_FLOAT_EQ(result.at(3,1), 4.0f); 
        ASSERT_FLOAT_EQ(result.at(3,2), 6.0f); 
    }

}

TEST_F(Matrix4x4Test, UnitRotationMatrix)
{
    const float epsilon = 0.0001f;
    float radiansQuarterTurn = (float) M_PI / 2.0f;  
    {
        Vec3f unitX(1.0f, 0.0f, 0.0f);
        Matrix4x4 m = Matrix4x4::rotationMatrix(radiansQuarterTurn, 0.0f, 0.0f);
        Vec3f result = transformCoord(m, unitX);
        ASSERT_NEAR(result.x,  0.0f, epsilon); 
        ASSERT_NEAR(result.y, -1.0f, epsilon); 
        ASSERT_NEAR(result.z,  0.0, epsilon); 
    }

    {
        Vec3f unitX(1.0f, 0.0f, 0.0f);
        Matrix4x4 m = Matrix4x4::rotationMatrix(0.0f, radiansQuarterTurn, 0.0f);
        Vec3f result = transformCoord(m, unitX);
        ASSERT_NEAR(result.x,  1.0f, epsilon); 
        ASSERT_NEAR(result.y,  0.0f, epsilon); 
        ASSERT_NEAR(result.z,  0.0, epsilon); 
    }

    {
        Vec3f unitX(1.0f, 0.0f, 0.0f);
        Matrix4x4 m = Matrix4x4::rotationMatrix(0.0f, 0.0f, radiansQuarterTurn);
        Vec3f result = transformCoord(m, unitX);
        ASSERT_NEAR(result.x,  0.0f, epsilon); 
        ASSERT_NEAR(result.y, -1.0f, epsilon); 
        ASSERT_NEAR(result.z,  0.0, epsilon); 
    }

    const float radians45Turn = (float) M_PI / 4.0f;  
    const float oneOverSqrt2 = 1.0f / sqrtf(2.0f);

    {
        Vec3f unitX(1.0f, 0.0f, 0.0f);
        Matrix4x4 m = Matrix4x4::rotationMatrix(radians45Turn, 0.0f, 0.0f);
        Vec3f result = transformCoord(m, unitX);
        ASSERT_NEAR(result.x,  oneOverSqrt2, epsilon); 
        ASSERT_NEAR(result.y, -oneOverSqrt2, epsilon); 
        ASSERT_NEAR(result.z,  0.0f,         epsilon); 
    }

    {
        Vec3f unitX(1.0f, 0.0f, 0.0f);
        Matrix4x4 m = Matrix4x4::rotationMatrix(0.0f, radians45Turn, 0.0f);
        Vec3f result = transformCoord(m, unitX);
        ASSERT_NEAR(result.x,  1.0f, epsilon); 
        ASSERT_NEAR(result.y,  0.0f, epsilon); 
        ASSERT_NEAR(result.z,  0.0, epsilon); 
    }

    {
        Vec3f unitX(1.0f, 0.0f, 0.0f);
        Matrix4x4 m = Matrix4x4::rotationMatrix(0.0f, 0.0f, radians45Turn);
        Vec3f result = transformCoord(m, unitX);
        ASSERT_NEAR(result.x,  oneOverSqrt2, epsilon); 
        ASSERT_NEAR(result.y, -oneOverSqrt2, epsilon); 
        ASSERT_NEAR(result.z,  0.0, epsilon); 
    }
}

TEST_F(Matrix4x4Test, RotationFromYawPitchRoll)
{
    static const float EPS = 1e-5f;
    const Vec3f xAxis(1.0f, 0.0f, 0.0f);
    const Vec3f yAxis(0.0f, 1.0f, 0.0f);
    const Vec3f zAxis(0.0f, 0.0f, 1.0f);
    {
        Matrix4x4 m = Matrix4x4::getRotationFromYawPitchRoll(90.0f, 0.0f, 0.0f);
        assertNear(transformNormal(m, xAxis), -zAxis, EPS);
        assertNear(transformNormal(m, yAxis), yAxis, EPS);
        assertNear(transformNormal(m, zAxis), xAxis, EPS);
    }
    {
        Matrix4x4 m = Matrix4x4::getRotationFromYawPitchRoll(0.0f, 90.0f, 0.0f);
        assertNear(transformNormal(m, xAxis), xAxis, EPS);
        assertNear(transformNormal(m, yAxis), zAxis, EPS);
        assertNear(transformNormal(m, zAxis), -yAxis, EPS);
    }
    {
        Matrix4x4 m = Matrix4x4::getRotationFromYawPitchRoll(0.0f, 0.0f, 90.0f);
        assertNear(transformNormal(m, xAxis), yAxis, EPS);
        assertNear(transformNormal(m, yAxis), -xAxis, EPS);
        assertNear(transformNormal(m, zAxis), zAxis, EPS);
    }
}

TEST_F(Matrix4x4Test, ProjectionTest)
{
    static const float EPS = 1e-3f;
    Vec3f eyePos(1.1f, -1.2f, 0.3f);
    Vec3f projectionPlaneBasePos(0.7f, -0.2f, 1024.0f);
    Vec3f projectionPlaneDx(1.0f, 0.2f, -0.1f);
    Vec3f projectionPlaneDy(0.3f, 1.0f, 0.05f);
    Vec2ui projectionResolution(1017, 973);
    Matrix4x4 perspectiveProjection = Matrix4x4::getPerspectiveProjection(eyePos, projectionResolution, projectionPlaneBasePos, projectionPlaneDx, projectionPlaneDy);
    Matrix4x4 parallelProjection = Matrix4x4::getParallelProjection(eyePos, projectionPlaneBasePos, projectionPlaneDx, projectionPlaneDy);

    // check if every multiple of the delta vectors corresponds to the associated pixel
    for(unsigned int x = 0; x < projectionResolution.x; ++x)
    {
        for(unsigned int y = 0; y < projectionResolution.y; ++y)
        {
            auto p = projectionPlaneBasePos + ((float)x + 0.5f) * projectionPlaneDx + ((float)y + 0.5f) * projectionPlaneDy;
            auto transformedPerspective = transformCoord(perspectiveProjection, p);
            ASSERT_NEAR((float)x + 0.5f, transformedPerspective.x, EPS);
            ASSERT_NEAR((float)y + 0.5f, transformedPerspective.y, EPS);
            ASSERT_NEAR(1.0f, transformedPerspective.z, EPS);

            auto transformedParallel = transformCoord(parallelProjection, p);
            ASSERT_NEAR((float)x + 0.5f, transformedParallel.x, EPS);
            ASSERT_NEAR((float)y + 0.5f, transformedParallel.y, EPS);
            ASSERT_NEAR((eyePos - projectionPlaneBasePos).getLengthF(), transformedParallel.z, EPS);

            p = eyePos + ((float)x + 0.5f) * projectionPlaneDx + ((float)y + 0.5f) * projectionPlaneDy;
            transformedParallel = transformCoord(parallelProjection, p);
            ASSERT_NEAR((float)x + 0.5f, transformedParallel.x, EPS);
            ASSERT_NEAR((float)y + 0.5f, transformedParallel.y, EPS);
            ASSERT_NEAR(0.0f, transformedParallel.z, EPS);
        }
    }
}