#include "stdafx.h"
#include "Plane.h"
#include "framework/Logger.h"
#include "framework/error/GeometricComputationException.h"

namespace ettention
{
    static const float EPSILON = 1e-5f;
    static const float MINIMUM_NORMAL_LENGTH = 1e-4f;
    static const float MAXIMUM_DISTANCE_DELTA = 1e-3f;

    Plane Plane::FromPoints(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2)
    {
        return Plane::FromPointAndDirections(p0, p1 - p0, p2 - p0);
    }

    Plane Plane::FromPointAndDirections(const Vec3f& p, const Vec3f& u, const Vec3f& v)
    {
        return FromPointAndNormal(p, Cross(u, v));
    }

    Plane Plane::FromPointAndNormal(const Vec3f& point, const Vec3f& normal)
    {
        return FromNormalAndDistance(normal, -Dot(point, normal) / (float)normal.Length());
    }

    Plane Plane::FromNormalAndDistance(const Vec3f& normal, float dist)
    {
        float normalLength = normal.LengthF();
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

    Vec3f Plane::GetIntersectionWith(const Ray& ray) const
    {
        float dotvn = Dot(ray.GetDirection(), normal);
        if(fabs(dotvn) < EPSILON)
        {
            throw GeometricComputationException("Ray is parallel to plane!");
        }
        return ray.GetPointAt(-(Dot(ray.GetOrigin(), normal) + dist) / dotvn);
    }

    float Plane::GetDistanceTo(const Vec3f& point) const
    {
        return Dot(point, normal) + dist;
    }

    bool Plane::IsOnPlane(const Vec3f& point) const
    {
        return fabs(this->GetDistanceTo(point)) < MAXIMUM_DISTANCE_DELTA;
    }

    const Vec3f& Plane::GetNormal() const
    {
        return normal;
    }
}