#pragma once
#include <iostream>
#include <vector>
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class Matrix3x3
    {
    public:
        Matrix3x3();
        Matrix3x3(float c0x, float c0y, float c0z, float c1x, float c1y, float c1z, float c2x, float c2y, float c2z);
        Matrix3x3(Vec3f column0, Vec3f column1, Vec3f column2);
        Matrix3x3(const std::vector<float>& values);
        Matrix3x3(const Matrix3x3& other);

        Matrix3x3& operator=(const Matrix3x3& other);

        Matrix3x3 inverse();
        float determinant();

        const float& at(unsigned int column, unsigned int row) const;
        float& at(unsigned int column, unsigned int row);

        bool operator==(const Matrix3x3& other) const;

        static const Matrix3x3 identity;
        static Matrix3x3 translationMatrix(Vec2f& translate);
        static Matrix3x3 scaleMatrix(float v);
        static Matrix3x3 rotationMatrix(float phi);

    private:
        int indexOfMinor(int i, int droppedIndex);
        float determinantOfMinor(int x, int y);

        float values[9];
    };

    
    bool operator!=(const Matrix3x3& one, const Matrix3x3& other);

    Vec2f multiply(const Matrix3x3& matrix, const Vec2f& vector);
    Vec3f multiply(const Matrix3x3& matrix, const Vec3f& vector);
    Matrix3x3 multiply(const Matrix3x3& a, const Matrix3x3& b);

    std::istream &operator>>(std::istream& is, Matrix3x3& value);
    std::ostream &operator<<(std::ostream& os, Matrix3x3 value);
}
