#include "stdafx.h"
#include "FocalStackProjectionPattern.h"

namespace ettention
{
    namespace stem
    {
        FocalStackProjectionPattern::FocalStackProjectionPattern(float openingAngle,
                                                                 float minTiltAngle,
                                                                 float maxTiltAngle,
                                                                 float tiltAngleDistance,
                                                                 unsigned int focalImagesPerTilt,
                                                                 float volumeDepth,
                                                                 float detectorDistance)
                                                                 : openingAngle(openingAngle)
                                                                 , minTiltAngle(minTiltAngle)
                                                                 , maxTiltAngle(maxTiltAngle)
                                                                 , tiltAngleDistance(tiltAngleDistance)
                                                                 , focalImagesPerTilt(focalImagesPerTilt)
                                                                 , volumeDepth(volumeDepth)
                                                                 , detectorDistance(detectorDistance)
        {
        }

        FocalStackProjectionPattern::~FocalStackProjectionPattern()
        {
        }

        std::list<TFProjectionProperties> FocalStackProjectionPattern::getProjectionMetaData()
        {
            float focalSliceDepth;
            if(focalImagesPerTilt > 1)
                focalSliceDepth = volumeDepth / (float)(focalImagesPerTilt - 1);
            else
                focalSliceDepth = boost::numeric::bounds<float>::highest();

            float minFocalDistance = -(volumeDepth / 2.0f);
            const float epsilon = 0.001f;
            float maxFocalDistance = (volumeDepth / 2.0f) + epsilon; // add epsilon to include last slice

            std::list<TFProjectionProperties> projections;
            for(float tiltPosition = minTiltAngle; tiltPosition <= maxTiltAngle; tiltPosition += tiltAngleDistance)
            {
                if(focalImagesPerTilt > 1)
                {
                    for(float focalDistance = minFocalDistance; focalDistance <= maxFocalDistance; focalDistance += focalSliceDepth)
                    {
                        projections.push_back(TFProjectionProperties(tiltPosition, focalDistance, focalSliceDepth, openingAngle));
                        std::cout << "focal Distance " << focalDistance << std::endl;
                    }
                }
                else
                {
                    float focalDistance = detectorDistance;
                    projections.push_back(TFProjectionProperties(tiltPosition, focalDistance, focalSliceDepth, openingAngle));
                }
            }
            return projections;
        }
    }
}