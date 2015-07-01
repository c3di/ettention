#include "stdafx.h"

#include "Marker.h"

namespace ettention
{
    namespace stem
    {
        Marker::Marker(int id, float tiltAngle, Vec2f positionInImage)
            : id(id)
            , tiltAngle(tiltAngle)
            , positionInImage(positionInImage)
            , ray(Ray::FromPointAndDirection(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(1.0f, 0.0f, 0.0f)))
        {
        }

        Marker::~Marker()
        {
        }

        int Marker::getId() const
        {
            return id;
        }

        void Marker::setId(int val)
        {
            id = val;
        }

        Vec2f Marker::PositionInImage() const
        {
            return positionInImage;
        }

        void Marker::PositionInImage(Vec2f val)
        {
            positionInImage = val;
        }

        std::vector<float>& Marker::IntensityZProjection()
        {
            return intensityZProjection;
        }

        float Marker::getIntensityMaximum()
        {
            float maximum = 0.0f;
            for(unsigned int i = 0; i < intensityZProjection.size(); i++)
            if(intensityZProjection[i] > maximum)
                maximum = intensityZProjection[i];
            return maximum;
        }

        float Marker::TiltAngle() const
        {
            return tiltAngle;
        }

        void Marker::TiltAngle(float val)
        {
            tiltAngle = val;
        }

        unsigned int Marker::getNumberOfLocalMaxima()
        {
            unsigned int maximaCount = 0;
            for(unsigned int i = 1; i < intensityZProjection.size() - 1; i++)
            {
                if((intensityZProjection[i] > intensityZProjection[i - 1]) && (intensityZProjection[i] > intensityZProjection[i + 1]))
                    maximaCount++;
            }
            return maximaCount;
        }

        unsigned int Marker::getPositionOfGlobalMaximum()
        {
            float maximum = 0.0f;
            unsigned int maximumPosition = 0;
            for(unsigned int i = 0; i < intensityZProjection.size(); i++)
            {
                if(intensityZProjection[i] > maximum)
                {
                    maximum = intensityZProjection[i];
                    maximumPosition = i;
                }
            }
            return (unsigned int)intensityZProjection.size() - maximumPosition - 1;
        }

        Ray Marker::getRay()
        {
            return ray;
        }

        void Marker::setRay(Ray val)
        {
            ray = val;
        }

    }
}