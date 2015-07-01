#pragma once
#include "ProjectionPattern.h"

namespace ettention
{
    namespace stem
    {
        class SpiralProjectionPattern : public ProjectionPattern
        {
        public:
            SpiralProjectionPattern(float openingAngle, float minTiltAngle, float maxTiltAngle, float tiltAngleDistance, unsigned int nFocalPositions, float volumeDepth);
            ~SpiralProjectionPattern();

            std::list<TFProjectionProperties> getProjectionMetaData() override;

        protected:
            float openingAngle;
            float minTiltAngle;
            float maxTiltAngle;
            float tiltAngleDistance;
            unsigned int nFocalPositions;
            float volumeDepth;
        };
    }
}