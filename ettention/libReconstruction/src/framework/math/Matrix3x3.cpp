#include "stdafx.h"

#include "framework/math/Matrix3x3.h"
#include "framework/error/Exception.h"

namespace ettention
{
    const Matrix3x3 Matrix3x3::identity = Matrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    Matrix3x3::Matrix3x3(const std::vector<float>& values)
    {
        if(values.size() != 9)
        {
            throw Exception("Cannot initialize 3x3 Matrix from vector of size other than 9!");
        }
        memcpy(this->values, &values[0], 9 * sizeof(float));
    }

    Matrix3x3::Matrix3x3(float c0x, float c0y, float c0z, float c1x, float c1y, float c1z, float c2x, float c2y, float c2z)
    {
        at(0, 0) = c0x;
        at(0, 1) = c0y;
        at(0, 2) = c0z;
        at(1, 0) = c1x;
        at(1, 1) = c1y;
        at(1, 2) = c1z;
        at(2, 0) = c2x;
        at(2, 1) = c2y;
        at(2, 2) = c2z;
    }

    Matrix3x3::Matrix3x3(Vec3f column0, Vec3f column1, Vec3f column2)
    {
        at(0, 0) = column0.x;
        at(0, 1) = column0.y;
        at(0, 2) = column0.z;
        at(1, 0) = column1.x;
        at(1, 1) = column1.y;
        at(1, 2) = column1.z;
        at(2, 0) = column2.x;
        at(2, 1) = column2.y;
        at(2, 2) = column2.z;
    }

    Matrix3x3::Matrix3x3(const Matrix3x3& other)
    {
        memcpy(values, other.values, 9 * sizeof(float));
    }

    Matrix3x3& Matrix3x3::operator=(const Matrix3x3& other)
    {
        memcpy(values, other.values, 9 * sizeof(float));
        return *this;
    }

    Matrix3x3 Matrix3x3::inverse()
    {
        float det = determinant();
        if(fabs(det) == 0.0f)
        {
            throw Exception("matrix not invertible");
        }
        float oneOverDeterminant = 1.0f / det;

        Matrix3x3 inverted;
        for(int y = 0; y < 3; y++)
        {
            for(int x = 0; x < 3; x++)
            {
                inverted.at(y, x) = ((x + y) % 2 == 1 ? -1 : 1) * determinantOfMinor(x, y) * oneOverDeterminant;
            }
        }
        return inverted;
    }

    float Matrix3x3::determinant()
    {
        return (this->at(0, 0) * determinantOfMinor(0, 0))
            - (this->at(0, 1) * determinantOfMinor(0, 1))
            + (this->at(0, 2) * determinantOfMinor(0, 2));
    }

    const float& Matrix3x3::at(unsigned int column, unsigned int row) const
    {
        return values[3 * column + row];
    }

    float& Matrix3x3::at(unsigned int column, unsigned int row)
    {
        return values[3 * column + row];
    }

    Matrix3x3 Matrix3x3::translationMatrix(Vec2f& translate)
    {
        Matrix3x3 m = identity;
        m.at(2, 0) = translate.x;
        m.at(2, 1) = translate.y;
        return m;
    }

    Matrix3x3 Matrix3x3::scaleMatrix(float v)
    {
        Matrix3x3 m = identity;
        m.at(0, 0) = v;
        m.at(1, 1) = v;
        return m;
    }

    Matrix3x3 Matrix3x3::rotationMatrix(float phi)
    {
        const float cphi = cosf(phi);
        const float sphi = sinf(phi);
        Matrix3x3 m = Matrix3x3::identity;
        m.at(0, 0) = cphi;
        m.at(1, 0) = -sphi;
        m.at(0, 1) = sphi;
        m.at(1, 1) = cphi;
        return m;
    }

    int Matrix3x3::indexOfMinor(int i, int droppedIndex)
    {
        if(i < droppedIndex)
            return i;
        return i + 1;
    }

    float Matrix3x3::determinantOfMinor(int x, int y)
    {
        int r[2];
        for(int i = 0; i < 2; i++)
            r[i] = indexOfMinor(i, x);
        int c[2];
        for(int i = 0; i < 2; i++)
            c[i] = indexOfMinor(i, y);

        return at(r[0], c[0]) * at(r[1], c[1]) -
            at(r[0], c[1]) * at(r[1], c[0]);
    }

    bool Matrix3x3::operator==(const Matrix3x3& other) const
    {
        for(int i = 0; i < 9; i++)
        {
            if(values[i] != other.values[i])
            {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix3x3& one, const Matrix3x3& other)
    {
        return !(one == other);
    }

    Matrix3x3::Matrix3x3()
    {
    }

    Vec3f multiply(const Matrix3x3& m, const Vec3f& v)
    {
        const float x = m.at(0, 0)*v.x + m.at(1, 0)*v.y + m.at(2, 0)*v.z;
        const float y = m.at(0, 1)*v.x + m.at(1, 1)*v.y + m.at(2, 1)*v.z;
        const float z = m.at(0, 2)*v.x + m.at(1, 2)*v.y + m.at(2, 2)*v.z;
        return Vec3f(x, y, z);
    }

    Vec2f multiply(const Matrix3x3& m, const Vec2f& v)
    {
        Vec3f v3(v.x, v.y, 1.0f);
        v3 = multiply(m, v3);
        return Vec2f(v3.x / v3.z, v3.y / v3.z);
    }

    Matrix3x3 multiply(const Matrix3x3& a, const Matrix3x3& b)
    {
        Matrix3x3 m;
        for(unsigned int i = 0; i < 3; i++)
        {
            for(unsigned int j = 0; j < 3; j++)
            {
                m.at(i, j) = 0.0f;
                for(unsigned int k = 0; k < 3; k++)
                    m.at(i, j) += a.at(k, j)*b.at(i, k);
            }
        }
        return m;
    }

    std::istream &operator>>(std::istream& is, Matrix3x3& value)
    {
        return is;
    }

    std::ostream &operator<<(std::ostream& os, Matrix3x3 value)
    {
        os << "(";
        for(unsigned int i = 0; i < 3; i++)
        {
            for(unsigned int j = 0; j < 3; j++)
            {
                os << value.at(i, j) << " ";
            }
            os << "| ";
        }
        os << ")";
        return os;
    }
}
