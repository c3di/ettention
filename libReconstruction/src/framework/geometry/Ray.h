#pragma once
#include <iostream>
#include "framework/math/Vec3.h"

namespace ettention
{
    class Ray
    {
    public:
        static Ray FromPoints(const Vec3f& p0, const Vec3f& p1);
        static Ray FromPointAndDirection(const Vec3f& p, const Vec3f& dir);

        virtual ~Ray();

        void SetDirection(const Vec3f& direction);
        const Vec3f& GetDirection() const;
        void SetOrigin(const Vec3f& origin);
        const Vec3f& GetOrigin() const;
        Vec3f GetPointAt(float t) const;

        Vec3f GetInvertedRayDirection();

    private:
        Ray(const Vec3f& origin, const Vec3f& direction);

        Vec3f origin;
        Vec3f direction;
    };

    std::ostream& operator<<(std::ostream& oss, const Ray& ray);
}