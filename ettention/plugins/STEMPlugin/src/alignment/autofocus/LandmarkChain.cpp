#include "stdafx.h"

#include "LandmarkChain.h"

namespace ettention
{
    namespace stem
    {
        LandmarkChain::LandmarkChain()
        {
        }

        LandmarkChain::~LandmarkChain()
        {
        }

        void LandmarkChain::addEntryForTiltAngle(float tiltAngle, std::shared_ptr<Marker> marker)
        {
            markerByTiltAngle[tiltAngle] = marker;
        }

        std::shared_ptr<Marker> LandmarkChain::getEntryForTiltAngle(float tiltAngle)
        {
            return markerByTiltAngle[tiltAngle];
        }

        bool LandmarkChain::hasEntryForTiltAngle(float tiltAngle)
        {
            return markerByTiltAngle.find(tiltAngle) != markerByTiltAngle.end();
        }

        std::map< float, std::shared_ptr<Marker> >::iterator LandmarkChain::begin()
        {
            return markerByTiltAngle.begin();
        }

        std::map< float, std::shared_ptr<Marker> >::iterator LandmarkChain::end()
        {
            return markerByTiltAngle.end();
        }

        float LandmarkChain::getAlignmentError() const
        {
            return alignmentError;
        }

        void LandmarkChain::getAlignmentError(float val)
        {
            alignmentError = val;
        }

        Vec3f LandmarkChain::getParticlePosition() const
        {
            return particlePosition;
        }

        void LandmarkChain::getParticlePosition(Vec3f val)
        {
            particlePosition = val;
        }
    }
}