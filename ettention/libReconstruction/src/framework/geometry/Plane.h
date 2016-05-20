#pragma once
#include "framework/geometry/Ray.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class Plane 
    {
    private:
        Vec3f normal;
        float dist;

        Plane(const Vec3f& normal, float dist);

    public:
        static Plane makePlaneFromNormalAndDistance(const Vec3f& normal, float dist);
        static Plane makePlaneFromPointAndNormal(const Vec3f& point, const Vec3f& normal);
        static Plane makePlaneFromPoints(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2);
        static Plane makePlaneFromPointAndDirections(const Vec3f& p, const Vec3f& u, const Vec3f& v);

        virtual ~Plane();

        Vec3f getIntersectionWith(const Ray& ray) const;
        float getDistanceTo(const Vec3f& point) const;
        bool isOnPlane(const Vec3f& point) const;
        const Vec3f& getNormal() const;
    };
}

