#pragma once
#include "ProjectionPattern.h"

namespace ettention
{
    namespace stem
    {
        class FocalStackProjectionPattern : public ProjectionPattern
        {
        public:
            FocalStackProjectionPattern(float openingAngle, float minTiltAngle, float maxTiltAngle, float tiltAngleDistance, unsigned int focalImagesPerTilt, float volumeDepth, float detectorDistance);
            ~FocalStackProjectionPattern();

            std::list<TFProjectionProperties> getProjectionMetaData() override;

        protected:
            float openingAngle;
            float minTiltAngle;
            float maxTiltAngle;
            float tiltAngleDistance;
            unsigned int focalImagesPerTilt;
            float volumeDepth;
            float detectorDistance;
        };
    }
}