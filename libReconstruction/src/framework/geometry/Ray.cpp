#include "stdafx.h"
#include "Ray.h"
#include "framework/Logger.h"

namespace ettention
{
    static const float MINIMUM_DIRECTION_LENGTH = 1e-6f;

    Ray Ray::FromPoints(const Vec3f& p0, const Vec3f& p1)
    {
        return Ray(p0, p1 - p0);
    }

    Ray Ray::FromPointAndDirection(const Vec3f& p, const Vec3f& dir)
    {
        return Ray(p, dir);
    }

    Ray::Ray(const Vec3f& origin, const Vec3f& direction)
        : origin(0.0f, 0.0f, 0.0f)
        , direction(0.0f, 0.0f, 0.0f)
    {
        this->SetOrigin(origin);
        this->SetDirection(direction);
    }

    Ray::~Ray()
    {
    }

    const Vec3f& Ray::GetDirection() const
    {
        return direction;
    }

    void Ray::SetDirection(const Vec3f& direction)
    {
        if(direction.Length() < MINIMUM_DIRECTION_LENGTH)
        {
            LOGGER("Ray direction length is too small.");
        }
        else
        {
            this->direction = Normalize(direction);
        }
    }

    const Vec3f& Ray::GetOrigin() const
    {
        return origin;
    }

    void Ray::SetOrigin(const Vec3f& origin)
    {
        this->origin = origin;
    }

    Vec3f Ray::GetPointAt(float t) const
    {
        return origin + t * direction;
    }

    Vec3f Ray::GetInvertedRayDirection()
    {
        Vec3f invRay;
        for(unsigned int dim = 0; dim < 3; dim++)
        {
            if(direction[dim] != 0.0f)
                invRay[dim] = 1.0f / direction[dim];
            else
                invRay[dim] = std::numeric_limits<float>::max();
        }
        return invRay;
    }


    std::ostream& operator<<(std::ostream& oss, const Ray& ray)
    {
        oss << "Ray = (" << ray.GetOrigin() << ") + t * (" << ray.GetDirection() << ")";
        return oss;
    }
}