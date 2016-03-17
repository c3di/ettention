#include "stdafx.h"
#include "Ray.h"
#include "framework/Logger.h"

namespace ettention
{
    static const float MINIMUM_DIRECTION_LENGTH = 1e-6f;

    Ray Ray::createFromPoints(const Vec3f& p0, const Vec3f& p1)
    {
        return Ray(p0, p1 - p0);
    }

    Ray Ray::createFromPointAndDirection(const Vec3f& p, const Vec3f& dir)
    {
        return Ray(p, dir);
    }

    Ray::Ray(const Vec3f& origin, const Vec3f& direction)
        : origin(0.0f, 0.0f, 0.0f)
        , direction(0.0f, 0.0f, 0.0f)
    {
        setOrigin(origin);
        setDirection(direction);
    }

    Ray::~Ray()
    {
    }

    const Vec3f& Ray::getNormalizedDirection() const
    {
        return direction;
    }

    void Ray::setDirection(const Vec3f& direction)
    {
        if(direction.getLength() < MINIMUM_DIRECTION_LENGTH)
        {
            LOGGER("Ray direction length is too small.");
        }
        else
        {
            this->direction = doNormalize(direction);
        }
    }

    const Vec3f& Ray::getOrigin() const
    {
        return origin;
    }

    void Ray::setOrigin(const Vec3f& origin)
    {
        this->origin = origin;
    }

    Vec3f Ray::getPointAt(float t) const
    {
        return origin + t * direction;
    }

    Vec3f Ray::getInvertedNormalizedDirection() const
    {
        Vec3f invRay;
        for(unsigned int dim = 0; dim < 3; dim++)
        {
            if(direction[dim] != 0.0f)
            {
                invRay[dim] = 1.0f / direction[dim];
            }
            else
            {
                invRay[dim] = std::numeric_limits<float>::max();
            }
        }
        return invRay;
    }


    std::ostream& operator<<(std::ostream& oss, const Ray& ray)
    {
        oss << "Ray = (" << ray.getOrigin() << ") + t * (" << ray.getNormalizedDirection() << ")";
        return oss;
    }
}