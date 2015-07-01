#include "stdafx.h"
#include "framework/Logger.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/Exception.h"
#include "framework/error/GeometricComputationException.h"
#include "framework/math/Matrix3x3.h"

namespace ettention
{
    float NumericalAlgorithms::degreeToRadians(float degree)
    {
        return degree / 180.0f * (float)M_PI;
    }

    float NumericalAlgorithms::radiansToDegree(float radians)
    {
        return radians / (float)M_PI * 180.0f;
    }

    unsigned int NumericalAlgorithms::nearestPowerOfTwo(unsigned int number)
    {
        unsigned int v = number;
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }

    Vec2ui NumericalAlgorithms::nearestPowerOfTwo(Vec2ui resolution)
    {
        return Vec2ui(NumericalAlgorithms::nearestPowerOfTwo(resolution.x), NumericalAlgorithms::nearestPowerOfTwo(resolution.y));
    }


    unsigned long long NumericalAlgorithms::factorial(unsigned int n)
    {
        return n < 2 ? (unsigned long long)1 : (unsigned long long)n * factorial(n - 1);
    }

    double NumericalAlgorithms::uintPow(double base, unsigned int exponent)
    {
        if(exponent == 0)
        {
            return 1.0;
        }
        else if(exponent == 1)
        {
            return base;
        }
        else
        {
            double factor = uintPow(base, exponent / 2);
            return factor * factor * (exponent % 2 == 0 ? 1.0 : base);
        }
        return exponent == 0 ? 1.0 : base * uintPow(base, exponent - 1);
    }

    float NumericalAlgorithms::GetIntersectionLengthOfRayThroughVoxelCenter(const Vec3f& rayDir, const Vec3f& voxelSize)
    {
        if(rayDir.x != 0.0f)
        {
            float t = -0.5f * voxelSize.x / rayDir.x;
            Vec3f intersection = t * rayDir;
            if(-0.5f * voxelSize.y <= intersection.y && intersection.y <= 0.5f * voxelSize.y &&
               -0.5f * voxelSize.z <= intersection.z && intersection.z <= 0.5f * voxelSize.z)
            {
                return 2.0f * std::abs(t) * rayDir.LengthF();
            }
        }
        if(rayDir.y != 0.0f)
        {
            float t = -0.5f * voxelSize.y / rayDir.y;
            Vec3f intersection = t * rayDir;
            if(-0.5f * voxelSize.x <= intersection.x && intersection.x <= 0.5f * voxelSize.x &&
               -0.5f * voxelSize.z <= intersection.z && intersection.z <= 0.5f * voxelSize.z)
            {
                return 2.0f * std::abs(t) * rayDir.LengthF();
            }
        }
        if(rayDir.z != 0.0f)
        {
            float t = -0.5f * voxelSize.z / rayDir.z;
            Vec3f intersection = t * rayDir;
            if(-0.5f * voxelSize.y <= intersection.y && intersection.y <= 0.5f * voxelSize.y &&
               -0.5f * voxelSize.x <= intersection.x && intersection.x <= 0.5f * voxelSize.x)
            {
                return 2.0f * std::abs(t) * rayDir.LengthF();
            }
        }
        throw GeometricComputationException("No intersection of ray with voxel sides!");
    }

    std::string NumericalAlgorithms::bytesToString(std::size_t bytes)
    {
        std::stringstream str;
        if(bytes > ((std::size_t)2 << 40))
            str << std::setprecision(3) << (float)bytes / (float)((std::size_t)1 << 40) << " TiB";
        else if(bytes > ((std::size_t)2 << 30))
            str << std::setprecision(3) << (float)bytes / (float)((std::size_t)1 << 30) << " GiB";
        else if(bytes > ((std::size_t)2 << 20))
            str << std::setprecision(3) << (float)bytes / (float)((std::size_t)1 << 20) << " MiB";
        else if(bytes > ((std::size_t)2 << 10))
            str << std::setprecision(3) << (float)bytes / (float)((std::size_t)1 << 10) << " KiB";
        else
            str << bytes << " B";
        return str.str();
    }
}