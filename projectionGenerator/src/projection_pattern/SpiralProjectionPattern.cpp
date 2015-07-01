#include "stdafx.h"
#include "SpiralProjectionPattern.h"

namespace ettention
{
    namespace stem
    {
        SpiralProjectionPattern::SpiralProjectionPattern(float openingAngle, float minTiltAngle, float maxTiltAngle, float tiltAngleDistance, unsigned int nFocalPositions, float volumeDepth)
            : openingAngle(openingAngle)
            , minTiltAngle(minTiltAngle)
            , maxTiltAngle(maxTiltAngle)
            , tiltAngleDistance(tiltAngleDistance)
            , nFocalPositions(nFocalPositions)
            , volumeDepth(volumeDepth)
        {
        }

        SpiralProjectionPattern::~SpiralProjectionPattern()
        {
        }

        std::list<TFProjectionProperties> SpiralProjectionPattern::getProjectionMetaData()
        {
            float focalSliceDepth = volumeDepth / (float)(nFocalPositions + 1);

            float minFocalDistance = (128.0f - (volumeDepth / 2.0f)) + focalSliceDepth;
            float maxFocalDistance = (128.0f + (volumeDepth / 2.0f)) - focalSliceDepth;

            const float epsilon = 0.1f;

            std::list<TFProjectionProperties> projections;
            float focalDistance = minFocalDistance;

            for(float tiltPosition = minTiltAngle; tiltPosition <= maxTiltAngle; tiltPosition += tiltAngleDistance)
            {
                projections.push_back(TFProjectionProperties(tiltPosition, focalDistance, focalSliceDepth, openingAngle));
                focalDistance += focalSliceDepth;
                if(focalDistance > maxFocalDistance + epsilon)
                    focalDistance = minFocalDistance;
            }
            return projections;
        }
    }
}