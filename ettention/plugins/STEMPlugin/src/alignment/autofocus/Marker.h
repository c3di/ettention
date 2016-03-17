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

            Vec2f getPositionInImage() const;
            void getPositionInImage(Vec2f val);

            std::vector<float>& getIntensityZProjection();
            float getIntensityMaximum();

            float getTiltAngle() const;
            void getTiltAngle(float val);

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