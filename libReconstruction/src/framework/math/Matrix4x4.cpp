#include "stdafx.h"
#include <math.h>
#include "framework/math/Matrix4x4.h"
#include "framework/error/Exception.h"

namespace ettention 
{

    const Matrix4x4 Matrix4x4::identity = Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    Matrix4x4::Matrix4x4(std::vector<float> v) 
        : values(16)
    {
        if(v.size() != 16)
            throw Exception("cannot initialize 4x4 Matrix from vector of size other than 16");
        for(int i = 0; i < 16; i++)
            values[i] = v[i];
    }
    
    Matrix4x4::Matrix4x4(float c0x, float c1x, float c2x, float c3x,
                         float c0y, float c1y, float c2y, float c3y,
                         float c0z, float c1z, float c2z, float c3z,
                         float c0w, float c1w, float c2w, float c3w)
        : values(16)
    {
        at(0,0) = c0x;
        at(0,1) = c0y;
        at(0,2) = c0z;
        at(0,3) = c0w;
        at(1,0) = c1x;
        at(1,1) = c1y;
        at(1,2) = c1z;
        at(1,3) = c1w;
        at(2,0) = c2x;
        at(2,1) = c2y;
        at(2,2) = c2z;
        at(2,3) = c2w;
        at(3,0) = c3x;
        at(3,1) = c3y;
        at(3,2) = c3z;
        at(3,3) = c3w;
    }
    
    Matrix4x4::Matrix4x4(Vec4f column0, Vec4f column1, Vec4f column2, Vec4f column3)
        : values(16)
    {
        at(0,0) = column0.x;
        at(0,1) = column0.y;
        at(0,2) = column0.z;
        at(0,3) = column0.w;
        at(1,0) = column1.x;
        at(1,1) = column1.y;
        at(1,2) = column1.z;
        at(1,3) = column1.w;
        at(2,0) = column2.x;
        at(2,1) = column2.y;
        at(2,2) = column2.z;
        at(2,3) = column2.w;
        at(3,0) = column3.x;
        at(3,1) = column3.y;
        at(3,2) = column3.z;
        at(3,3) = column3.w;
    }
    
    Matrix4x4::Matrix4x4(const Matrix4x4& other)
        : values(16)
    {
        values = other.values;
    }
    
    Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other)
    {
        values = other.values;
        return *this;
    }
    
    Matrix4x4 Matrix4x4::transpose()
    {
        return Matrix4x4(values[ 0], values[ 4], values[ 8], values[12], 
                          values[ 1], values[ 5], values[ 9], values[13], 
                          values[ 2], values[ 6], values[10], values[14], 
                          values[ 3], values[ 7], values[11], values[15]);
    }

    Matrix4x4 Matrix4x4::inverse()
    {
        float det = determinant();
        if(fabs(det) == 0.0f)
        {
            throw Exception("matrix not invertible");
        }
        float oneOverDeterminant = 1.0f / det;
        Matrix4x4 inverted;
        for(int c = 0; c < 4; c++)
        {
            for(int r = 0; r < 4; r++)
            {
                inverted.at(r, c) = determinantOfMinor(c, r) * oneOverDeterminant;
                if((r + c) % 2 == 1)
                {
                    inverted.at(r, c) *= -1.0f;
                }
            }
        }
        return inverted;
    }

    float Matrix4x4::determinant()
    {
        return (this->at(0, 0) * determinantOfMinor(0, 0))
             - (this->at(0, 1) * determinantOfMinor(0, 1))
             + (this->at(0, 2) * determinantOfMinor(0, 2))
             - (this->at(0, 3) * determinantOfMinor(0, 3));
    }

    const float& Matrix4x4::at(unsigned int column, unsigned int row) const
    {
        return values[ 4*column + row ];
    }

    float& Matrix4x4::at(unsigned int column, unsigned int row)
    {
        return values[ 4*column + row ];
    }

    Matrix4x4 Matrix4x4::translationMatrix(const Vec3f& translate)
    {
        Matrix4x4 m = identity;
        m.at(3, 0) = translate.x;
        m.at(3, 1) = translate.y;
        m.at(3, 2) = translate.z;
        return m;
    }

    Matrix4x4 Matrix4x4::scaleMatrix(float v)
    {
        Matrix4x4 m = identity;
        m.at(0, 0) = v;
        m.at(1, 1) = v;
        m.at(2, 2) = v;
        return m;
    }

    Matrix4x4 Matrix4x4::scaleMatrix(const Vec3f& s)
    {
        Matrix4x4 m = identity;
        m.at(0, 0) = s.x;
        m.at(1, 1) = s.y;
        m.at(2, 2) = s.z;
        return m;
    }

    Matrix4x4 Matrix4x4::rotationMatrix(float phi, float theta, float psi)
    {
        const float cphi   = cosf(phi);
        const float sphi   = sinf(phi);
        const float ctheta = cosf(theta);
        const float stheta = sinf(theta);
        const float cpsi   = cosf(psi);
        const float spsi   = sinf(psi);
        Matrix4x4 m = Matrix4x4::identity;
        m.at(0,0) = cphi*cpsi-ctheta*sphi*spsi;
        m.at(1,0) = cpsi*sphi+ctheta*cphi*spsi;
        m.at(2,0) = spsi*stheta;
        m.at(0,1) = -spsi*cphi-ctheta*sphi*cpsi;
        m.at(1,1) = -spsi*sphi+ctheta*cphi*cpsi;
        m.at(2,1) = cpsi*stheta;
        m.at(0,2) = stheta*sphi;
        m.at(1,2) = -stheta*cphi;
        m.at(2,2) = ctheta;
        return m;
    }

    Matrix4x4 Matrix4x4::RotationFromYawPitchRoll(float yawInDegree, float pitchInDegree, float rollInDegree)
    {
        const float phi = (float)M_PI * yawInDegree / 180.0f;
        const float theta = (float)M_PI * pitchInDegree / 180.0f;
        const float psi = (float)M_PI * rollInDegree / 180.0f;
        const float sPhi = std::sin(phi);
        const float cPhi = std::cos(phi);
        const float sTheta = std::sin(theta);
        const float cTheta = std::cos(theta);
        const float sPsi = std::sin(psi);
        const float cPsi = std::cos(psi);
        Matrix4x4 m = Matrix4x4::identity;
        m.at(0, 0) = cPhi * cPsi + sPhi * sTheta * sPsi;
        m.at(1, 0) = -cPhi * sPsi + sPhi * sTheta * cPsi;
        m.at(2, 0) = sPhi * cTheta;
        m.at(0, 1) = cTheta * sPsi;
        m.at(1, 1) = cTheta * cPsi;
        m.at(2, 1) = -sTheta;
        m.at(0, 2) = -sPhi * cPsi + cPhi * sTheta * sPsi;
        m.at(1, 2) = sPhi * sPsi + cPhi * sTheta * cPsi;
        m.at(2, 2) = cPhi * cTheta;
        return m;
    }

    void Matrix4x4::serializeToFloatArray(float* buffer)
    {
        memcpy(buffer, &values[0], 16 * sizeof(float));
    }

    float Matrix4x4::determinantOfMinor(int droppedColumn, int droppedRow)
    {
        int c[3];
        int r[3];
        for(int i = 0; i < 3; i++)
        {
            c[i] = i < droppedColumn ? i : i + 1;
            r[i] = i < droppedRow ? i : i + 1;
        }
        return at(c[0], r[0]) * (at(c[1], r[1]) * at(c[2], r[2]) - at(c[1], r[2]) * at(c[2], r[1])) -
               at(c[0], r[1]) * (at(c[1], r[0]) * at(c[2], r[2]) - at(c[1], r[2]) * at(c[2], r[0])) +
               at(c[0], r[2]) * (at(c[1], r[0]) * at(c[2], r[1]) - at(c[1], r[1]) * at(c[2], r[0]));
    }

    Matrix4x4::Matrix4x4(ResolutionType resolution)
        : values(16)
    {
        if(resolution != ResolutionType(4, 4))
            throw Exception("cannot initialize 4x4Matrix class from resolution other than 4x4");
    }

    Matrix4x4::Matrix4x4(Vec3ui resolution)
        : values(16)
    {
        if(resolution != Vec3ui(4, 4,1))
            throw Exception("cannot initialize 4x4Matrix class from resolution other than 4x4");
    }

    const Vec2ui& Matrix4x4::getResolution() const
    {
        static const Vec2ui resolution(4, 4);
        return resolution;
    }

    float* Matrix4x4::getData()
    {
        return  &values[0];
    }

    const float* Matrix4x4::getData() const
    {
        return &values[0];
    }

    std::size_t Matrix4x4::getByteWidth() const
    {
        return 16 * sizeof(float);
    }

    bool operator==(const Matrix4x4& one, const Matrix4x4& other)
    {
        for(int i = 0; i < 16; i++)
        {
            if(one.values[i] != other.values[i])
            {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix4x4& one, const Matrix4x4& other)
    {
        return ! (one == other);
    }

    Matrix4x4::Matrix4x4()
        : values(16)
    {
    }

    Vec3f TransformCoord(const Matrix4x4& m, const Vec3f& v)
    {
        const float x = m.at(0, 0) * v.x + m.at(1, 0) * v.y + m.at(2, 0) * v.z + m.at(3, 0);
        const float y = m.at(0, 1) * v.x + m.at(1, 1) * v.y + m.at(2, 1) * v.z + m.at(3, 1);
        const float z = m.at(0, 2) * v.x + m.at(1, 2) * v.y + m.at(2, 2) * v.z + m.at(3, 2);
        const float w = m.at(0, 3) * v.x + m.at(1, 3) * v.y + m.at(2, 3) * v.z + m.at(3, 3);
        return Vec3f(x / w, y / w, z / w);
    }

    Vec3f TransformNormal(const Matrix4x4& matrix, const Vec3f& vector)
    {
        return Vec3f(matrix.at(0, 0) * vector.x + matrix.at(1, 0) * vector.y + matrix.at(2, 0) * vector.z,
                     matrix.at(0, 1) * vector.x + matrix.at(1, 1) * vector.y + matrix.at(2, 1) * vector.z, 
                     matrix.at(0, 2) * vector.x + matrix.at(1, 2) * vector.y + matrix.at(2, 2) * vector.z);
    }

    void TransformCoordStream(const Matrix4x4& matrix, const Vec3f* sourceVectors, Vec3f* targetVectors, unsigned int vectorCount)
    {
        for(unsigned int i = 0; i < vectorCount; ++i)
        {
            targetVectors[i] = TransformCoord(matrix, sourceVectors[i]);
        }
    }

    Vec4f multiply(const Matrix4x4& m, const Vec4f& v)
    {
        const float x = m.at(0, 0)*v.x + m.at(1, 0)*v.y + m.at(2, 0)*v.z + m.at(3, 0)*v.w;
        const float y = m.at(0, 1)*v.x + m.at(1, 1)*v.y + m.at(2, 1)*v.z + m.at(3, 1)*v.w;
        const float z = m.at(0, 2)*v.x + m.at(1, 2)*v.y + m.at(2, 2)*v.z + m.at(3, 2)*v.w;
        const float w = m.at(0, 3)*v.x + m.at(1, 3)*v.y + m.at(2, 3)*v.z + m.at(3, 3)*v.w;
        return Vec4f(x, y, z, w);
    }

    Vec4f operator*(const Matrix4x4& left, const Vec4f& right)
    {
        return multiply(left, right);
    }

    Matrix4x4 multiply(const Matrix4x4& a, const Matrix4x4& b)
    {
        Matrix4x4 m;
        for(unsigned int i = 0; i < 4; i++)
        {
            for(unsigned int j = 0; j < 4; j++)
            {
                float v = 0.0f;
                for(unsigned int k = 0; k < 4; k++)
                {
                    v += a.at(k, j) * b.at(i, k);
                }
                m.at(i, j) = v;
            }
        }
        return m;
    }

    Matrix4x4 operator*(const Matrix4x4& left, const Matrix4x4& right)
    {
        return multiply(left, right);
    }

    std::istream &operator>>(std::istream& is, Matrix4x4& value)
    {
        return is;
    }

    std::ostream &operator<<(std::ostream& os, Matrix4x4 value)
    {
        os << "(";
        for(unsigned int i = 0; i < 4; i++) 
        {
            for(unsigned int j = 0; j < 4; j++) 
            {
                os << value.at(i, j) << " ";
            }
            os << "| ";
        }
        os << ")";
        return os;
    }

    void assignMatrixToGPUMappedVector(Matrix4x4& matrix, GPUMappedVector* vector)
    {
        vector->getObjectOnCPU()->at(0) = matrix.at(0, 0);
        vector->getObjectOnCPU()->at(1) = matrix.at(0, 1);
        vector->getObjectOnCPU()->at(2) = matrix.at(0, 2);
        vector->getObjectOnCPU()->at(3) = matrix.at(0, 3);
        vector->getObjectOnCPU()->at(4) = matrix.at(1, 0);
        vector->getObjectOnCPU()->at(5) = matrix.at(1, 1);
        vector->getObjectOnCPU()->at(6) = matrix.at(1, 2);
        vector->getObjectOnCPU()->at(7) = matrix.at(1, 3);
        vector->getObjectOnCPU()->at(8) = matrix.at(2, 0);
        vector->getObjectOnCPU()->at(9) = matrix.at(2, 1);
        vector->getObjectOnCPU()->at(10) = matrix.at(2, 2);
        vector->getObjectOnCPU()->at(11) = matrix.at(2, 3);
        vector->getObjectOnCPU()->at(12) = matrix.at(3, 0);
        vector->getObjectOnCPU()->at(13) = matrix.at(3, 1);
        vector->getObjectOnCPU()->at(14) = matrix.at(3, 2);
        vector->getObjectOnCPU()->at(15) = matrix.at(3, 3);
        vector->markAsChangedOnCPU();
    }

    Matrix4x4 Matrix4x4::PerspectiveProjection(const Vec3f& eyePos,
                                               const Vec3f& projectionPlaneBasePos,
                                               const Vec3f& projectionPlaneDx,
                                               const Vec3f& projectionPlaneDy,
                                               const Vec2ui& projectionPlaneResolution)
    {
        Vec3f projectionPlaneCenter = projectionPlaneBasePos +
                                      0.5f * (float)projectionPlaneResolution.x * projectionPlaneDx +
                                      0.5f * (float)projectionPlaneResolution.y * projectionPlaneDy;
        Vec3f viewDir = projectionPlaneCenter - eyePos;
        Matrix4x4 view = Matrix4x4(Vec4f(0.5f * (float)projectionPlaneResolution.x * projectionPlaneDx, 0.0f),
                                   Vec4f(0.5f * (float)projectionPlaneResolution.y * projectionPlaneDy, 0.0f),
                                   Vec4f(viewDir, 0.0f),
                                   Vec4f(eyePos, 1.0f)).inverse();
        Matrix4x4 proj(1.0f, 0.0f, 0.0f, 0.0f,
                       0.0f, 1.0f, 0.0f, 0.0f,
                       0.0f, 0.0f, 100.0f / 99.0f, -1.0f / 99.0f,
                       0.0f, 0.0f, 1.0f, 0.0f);
        Matrix4x4 coord(0.5f * (float)projectionPlaneResolution.x, 0.0f, 0.0f, 0.5f * ((float)projectionPlaneResolution.x - 1.0f),
                        0.0f, 0.5f * (float)projectionPlaneResolution.y, 0.0f, 0.5f * ((float)projectionPlaneResolution.y - 1.0f),
                        0.0f, 0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 0.0f, 1.0f);
        return coord * proj * view;
    }

    Matrix4x4 Matrix4x4::ParallelProjection(const Vec3f& frontPlaneBasePos,
                                            const Vec3f& backPlaneBasePos,
                                            const Vec3f& planeDx,
                                            const Vec3f& planeDy)
    {
        return Matrix4x4(Vec4f(planeDx, 0.0f),
                         Vec4f(planeDy, 0.0f),
                         Vec4f(Normalize(backPlaneBasePos - frontPlaneBasePos), 0.0f),
                         Vec4f(frontPlaneBasePos, 1.0f)).inverse();
    }
}
