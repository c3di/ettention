#pragma once
#include <utility>
#include "framework/geometry/Ray.h"
#include "framework/geometry/Plane.h"
#include "framework/geometry/BoundingBox.h"

namespace ettention
{

    class GeometricAlgorithms
    {
    public:
        static std::pair<Vec3f, Vec3f> GetClosestPointsOfRays(const Ray& a, const Ray& b);
        static float GetDistanceBetween(const Ray& a, const Ray& b);
        static float GetDistanceBetween(const Ray& a, const Vec3f& q);
        static Vec3f GetClosestPointOnPlane(const Vec3f& point, const Plane& plane);
        static std::pair<float,float> ClipRayAgainstBoundingBox(Ray ray, const BoundingBox3f& bb);
    };
 
}