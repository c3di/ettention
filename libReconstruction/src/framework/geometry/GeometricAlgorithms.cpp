#include "stdafx.h"
#include "framework/geometry/GeometricAlgorithms.h"
#include "framework/geometry/Plane.h"
#include "framework/error/GeometricComputationException.h"

namespace ettention
{
    const float EPSILON = 1e-5f;

    std::pair<Vec3f, Vec3f> GeometricAlgorithms::GetClosestPointsOfRays(const Ray& a, const Ray& b)
    {
        Vec3f connectingDirection = Cross(a.GetDirection(), b.GetDirection());
        if(connectingDirection.Length() < EPSILON)
        {
            throw GeometricComputationException("can not compute closestPoint of two parallel rays");
        }
        Plane planeContainingRayAParallelToConnectingDirection = Plane::FromPointAndDirections(a.GetOrigin(), connectingDirection, a.GetDirection());
        Plane planeContainingRayBParallelToConnectingDirection = Plane::FromPointAndDirections(b.GetOrigin(), connectingDirection, b.GetDirection());
        Vec3f pointA = planeContainingRayBParallelToConnectingDirection.GetIntersectionWith(a);
        Vec3f pointB = planeContainingRayAParallelToConnectingDirection.GetIntersectionWith(b);
        return std::pair<Vec3f, Vec3f>(pointA, pointB);
    }

    float GeometricAlgorithms::GetDistanceBetween(const Ray& a, const Ray& b)
    {
        auto points = GetClosestPointsOfRays(a, b);
        return (float)(points.first - points.second).Length();
    }

    float GeometricAlgorithms::GetDistanceBetween(const Ray& a, const Vec3f& q)
    {
        return (a.GetPointAt(Dot(q - a.GetOrigin(), a.GetDirection())) - q).LengthF();
    }

    Vec3f GeometricAlgorithms::GetClosestPointOnPlane(const Vec3f& point, const Plane& plane)
    {
        return point - plane.GetDistanceTo(point) * plane.GetNormal();
    }

    std::pair<float, float> GeometricAlgorithms::ClipRayAgainstBoundingBox(Ray ray, const BoundingBox3f& bb)
    {
        Vec3f invRay = ray.GetInvertedRayDirection();

        Vec3f t0 = (bb.GetMin() - ray.GetOrigin()) * invRay;
        Vec3f t1 = (bb.GetMax() - ray.GetOrigin()) * invRay;

        Vec3f tmin = ComponentWiseMin(t0, t1);
        Vec3f tmax = ComponentWiseMax(t0, t1);

        return std::make_pair(fmax(fmax(tmin.x, tmin.y), tmin.z),  fmin(fmin(tmax.x, tmax.y), tmax.z));
    }

}
