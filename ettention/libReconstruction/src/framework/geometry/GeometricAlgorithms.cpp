#include "stdafx.h"
#include "framework/geometry/GeometricAlgorithms.h"
#include "framework/geometry/Plane.h"
#include "framework/error/GeometricComputationException.h"

namespace ettention
{
    const float EPSILON = 1e-5f;

    std::pair<Vec3f, Vec3f> GeometricAlgorithms::getClosestPointsOfRays(const Ray& a, const Ray& b)
    {
        Vec3f connectingDirection = crossProduct(a.getNormalizedDirection(), b.getNormalizedDirection());
        if(connectingDirection.getLength() < EPSILON)
        {
            throw GeometricComputationException("can not compute closestPoint of two parallel rays");
        }
        Plane planeContainingRayAParallelToConnectingDirection = Plane::makePlaneFromPointAndDirections(a.getOrigin(), connectingDirection, a.getNormalizedDirection());
        Plane planeContainingRayBParallelToConnectingDirection = Plane::makePlaneFromPointAndDirections(b.getOrigin(), connectingDirection, b.getNormalizedDirection());
        Vec3f pointA = planeContainingRayBParallelToConnectingDirection.getIntersectionWith(a);
        Vec3f pointB = planeContainingRayAParallelToConnectingDirection.getIntersectionWith(b);
        return std::pair<Vec3f, Vec3f>(pointA, pointB);
    }

    float GeometricAlgorithms::getDistanceBetween(const Ray& a, const Ray& b)
    {
        auto points = getClosestPointsOfRays(a, b);
        return (float)(points.first - points.second).getLength();
    }

    float GeometricAlgorithms::getDistanceBetween(const Ray& a, const Vec3f& q)
    {
        return (a.getPointAt(dotProduct(q - a.getOrigin(), a.getNormalizedDirection())) - q).getLengthF();
    }

    Vec3f GeometricAlgorithms::getClosestPointOnPlane(const Vec3f& point, const Plane& plane)
    {
        return point - plane.getDistanceTo(point) * plane.getNormal();
    }

    std::pair<float, float> GeometricAlgorithms::getClipRayAgainstBoundingBox(Ray ray, const BoundingBox3f& bb)
    {
        Vec3f invRay = ray.getInvertedNormalizedDirection();

        Vec3f t0 = (bb.getMin() - ray.getOrigin()) * invRay;
        Vec3f t1 = (bb.getMax() - ray.getOrigin()) * invRay;

        Vec3f tmin = componentWiseMin(t0, t1);
        Vec3f tmax = componentWiseMax(t0, t1);

        return std::make_pair(fmax(fmax(tmin.x, tmin.y), tmin.z),  fmin(fmin(tmax.x, tmax.y), tmax.z));
    }

}
