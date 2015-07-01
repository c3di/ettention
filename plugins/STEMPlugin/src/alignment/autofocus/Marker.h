#pragma once

#include <vector>
#include "io/datasource/ImageStackDatasource.h"
#include "framework/geometry/Ray.h"

namespace ettention
{
    namespace stem
    {
        class Marker
        {
        public:
            Marker(int id, float tiltAngle, Vec2f positionInImage);
            virtual ~Marker();

            int getId() const;
            void setId(int val);

            Vec2f PositionInImage() const;
            void PositionInImage(Vec2f val);

            std::vector<float>& IntensityZProjection();
            float getIntensityMaximum();

            float TiltAngle() const;
            void TiltAngle(float val);

            unsigned int getNumberOfLocalMaxima();
            unsigned int getPositionOfGlobalMaximum();

            Ray getRay();
            void setRay(Ray val);

        protected:
            int id;
            Ray ray;
            float tiltAngle;
            Vec2f positionInImage;
            std::vector<float> intensityZProjection;
        };

    }
}