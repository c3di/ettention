#pragma once

#include <map>
#include "io/datasource/ImageStackDatasource.h"
#include "alignment/autofocus/Marker.h"

namespace ettention
{
    namespace stem
    {
        class LandmarkChain
        {
        public:
            LandmarkChain();
            virtual ~LandmarkChain();

            void addEntryForTiltAngle(float tiltAngle, std::shared_ptr<Marker> marker);
            std::shared_ptr<Marker> getEntryForTiltAngle(float tiltAngle);
            bool hasEntryForTiltAngle(float tiltAngle);

            float getAlignmentError() const;
            void getAlignmentError(float val);

            Vec3f getParticlePosition() const;
            void getParticlePosition(Vec3f val);

            std::map< float, std::shared_ptr<Marker> >::iterator begin();
            std::map< float, std::shared_ptr<Marker> >::iterator end();

        protected:
            std::map< float, std::shared_ptr<Marker> > markerByTiltAngle;
            float alignmentError;
            Vec3f particlePosition;
        };

    }
}