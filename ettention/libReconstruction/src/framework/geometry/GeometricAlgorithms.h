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
        static std::pair<Vec3f, Vec3f> getClosestPointsOfRays(const Ray& a, const Ray& b);
        static float getDistanceBetween(const Ray& a, const Ray& b);
        static float getDistanceBetween(const Ray& a, const Vec3f& q);
        static Vec3f getClosestPointOnPlane(const Vec3f& point, const Plane& plane);
        static std::pair<float,float> getClipRayAgainstBoundingBox(Ray ray, const BoundingBox3f& bb);
    };
 
}