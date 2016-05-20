#pragma once
#include <iostream>
#include <vector>
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"
#include "framework/math/Vec3.h"
#include "framework/math/Vec4.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    class Matrix4x4
    {
    public:
        Matrix4x4();
        Matrix4x4(float c0x, float c1x, float c2x, float c3x, float c0y, float c1y, float c2y, float c3y, float c0z, float c1z, float c2z, float c3z, float c0w, float c1w, float c2w, float c3w);
        Matrix4x4(Vec4f column0, Vec4f column1, Vec4f column2, Vec4f column3);
        Matrix4x4(std::vector<float> values);
        Matrix4x4(const Matrix4x4& other);

        Matrix4x4& operator=(const Matrix4x4& other);

        Matrix4x4 transpose();
        Matrix4x4 inverse();
        float determinant();
        const float& at(unsigned int column, unsigned int row) const;
        float& at(unsigned int column, unsigned int row);

        static const Matrix4x4 identity;
        static Matrix4x4 translationMatrix(const Vec3f& translate);
        static Matrix4x4 scaleMatrix(float v);
        static Matrix4x4 scaleMatrix(const Vec3f& s);
        static Matrix4x4 rotationMatrix(float phi, float theta, float psi);

        /**
         * left-handed coordinate system, y-axis up.
         * (1st: roll around z-axis, 2nd: pitch around x-axis, 3rd: yaw around y-axis)
         */
        static Matrix4x4 getRotationFromYawPitchRoll(float yawInDegree, float pitchInDegree, float rollInDegree);

        static Matrix4x4 getPerspectiveProjection(const Vec3f& sourcePos, const Vec2ui& resolution, const Vec3f& detectorBasePos, const Vec3f& detectorHorizontalPitch, const Vec3f& detectorVerticalPitch);
        static Matrix4x4 getParallelProjection(const Vec3f& frontPlaneBasePos, const Vec3f& backPlaneBasePos, const Vec3f& planeDx, const Vec3f& planeDy);

        void serializeToFloatArray(float* buffer);

        std::vector<float> values;

        // compatibility with GPUMapped<> template
        typedef Vec2ui ResolutionType;
        Matrix4x4(Vec2ui);
        Matrix4x4(Vec3ui);
        const Vec2ui& getResolution() const;
        float* getData();
        const float* getData() const;
        std::size_t getByteWidth() const;

    protected:
        float determinantOfMinor(int droppedColumn, int droppedRow);
    };

    bool operator==(const Matrix4x4& one, const Matrix4x4& other);
    bool operator!=(const Matrix4x4& one, const Matrix4x4& other);
    Matrix4x4 operator*(const Matrix4x4& left, const Matrix4x4& right);
    Vec4f operator*(const Matrix4x4& left, const Vec4f& right);

    /**
     * Multiplies the upper left 3x3 submatrix of the given matrix with the given vector.
     */
    Vec3f transformNormal(const Matrix4x4& matrix, const Vec3f& vector);

    /**
     * Assumes a homogeneous coordinate of 1 and does homogenization on the resulting vector.
     */
    Vec3f transformCoord(const Matrix4x4& matrix, const Vec3f& vector);

    void transformCoordStream(const Matrix4x4& matrix, const Vec3f* sourceVectors, Vec3f* targetVectors, unsigned int vectorCount);

    Vec4f multiply(const Matrix4x4& matrix, const Vec4f& vector);
    Matrix4x4 multiply(const Matrix4x4& a, const Matrix4x4& b);

    std::istream &operator>>(std::istream& is, Matrix4x4& value);
    std::ostream &operator<<(std::ostream& os, Matrix4x4 value);

    void assignMatrixToGPUMappedVector(Matrix4x4& matrix, GPUMappedVector* vector);
}
