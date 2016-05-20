#include "stdafx.h"
#include "Plane.h"
#include "framework/Logger.h"
#include "framework/error/GeometricComputationException.h"

namespace ettention
{
    static const float EPSILON = 1e-5f;
    static const float MINIMUM_NORMAL_LENGTH = 1e-4f;
    static const float MAXIMUM_DISTANCE_DELTA = 1e-3f;

    Plane Plane::makePlaneFromPoints(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2)
    {
        return Plane::makePlaneFromPointAndDirections(p0, p1 - p0, p2 - p0);
    }

    Plane Plane::makePlaneFromPointAndDirections(const Vec3f& p, const Vec3f& u, const Vec3f& v)
    {
        return makePlaneFromPointAndNormal(p, crossProduct(u, v));
    }

    Plane Plane::makePlaneFromPointAndNormal(const Vec3f& point, const Vec3f& normal)
    {
        return makePlaneFromNormalAndDistance(normal, -dotProduct(point, normal) / (float)normal.getLength());
    }

    Plane Plane::makePlaneFromNormalAndDistance(const Vec3f& normal, float dist)
    {
        float normalLength = normal.getLengthF();
        if(normalLength < MINIMUM_NORMAL_LENGTH)
        {
            throw GeometricComputationException("Normal of plane too small!");
        }
        return Plane(normal / normalLength, dist);
    }

    Plane::Plane(const Vec3f& normal, float dist)
        : normal(normal)
        , dist(dist)
    {
    }

    Plane::~Plane()
    {
    }

    Vec3f Plane::getIntersectionWith(const Ray& ray) const
    {
        float dotvn = dotProduct(ray.getNormalizedDirection(), normal);
        if(fabs(dotvn) < EPSILON)
        {
            throw GeometricComputationException("Ray is parallel to plane!");
        }
        return ray.getPointAt(-(dotProduct(ray.getOrigin(), normal) + dist) / dotvn);
    }

    float Plane::getDistanceTo(const Vec3f& point) const
    {
        return dotProduct(point, normal) + dist;
    }

    bool Plane::isOnPlane(const Vec3f& point) const
    {
        return fabs(this->getDistanceTo(point)) < MAXIMUM_DISTANCE_DELTA;
    }

    const Vec3f& Plane::getNormal() const
    {
        return normal;
    }
}