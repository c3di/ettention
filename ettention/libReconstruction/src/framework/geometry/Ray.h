#pragma once
#include <iostream>
#include "framework/math/Vec3.h"

namespace ettention
{
    class Ray
    {
    public:
        static Ray createFromPoints(const Vec3f& p0, const Vec3f& p1);
        static Ray createFromPointAndDirection(const Vec3f& p, const Vec3f& dir);

        virtual ~Ray();

        void setDirection(const Vec3f& direction);
        const Vec3f& getNormalizedDirection() const;
        void setOrigin(const Vec3f& origin);
        const Vec3f& getOrigin() const;
        Vec3f getPointAt(float t) const;

        Vec3f getInvertedNormalizedDirection() const;

    private:
        Ray(const Vec3f& origin, const Vec3f& direction);

        Vec3f origin;
        Vec3f direction;
    };

    std::ostream& operator<<(std::ostream& oss, const Ray& ray);
}