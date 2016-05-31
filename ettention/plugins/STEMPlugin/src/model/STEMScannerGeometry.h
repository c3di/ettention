#pragma once

#include "model/geometry/ParallelScannerGeometry.h"

namespace ettention
{
    class VariableContainer;

    namespace stem
    {
        class PLUGIN_API STEMScannerGeometry : public ParallelScannerGeometry
        {
        public:
            STEMScannerGeometry();
            STEMScannerGeometry(const STEMScannerGeometry* other);
            STEMScannerGeometry(ImageStackDatasource* inputStack);
            STEMScannerGeometry(const Vec2ui& resolution);
            STEMScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution, float tiltAngle, float focalDepth, float focalDifferenceBetweenImages, float confocalOpeningHalfAngle);
            virtual ~STEMScannerGeometry();

            virtual ScannerGeometry* clone() const override;

            void setTiltAngle(float tiltAngle);
            float getTiltAngle() const;

            void setFocalDepth(float focalDepth);
            float getFocalDepth() const;

            void setFocalDifferenceBetweenImages(float focalDifferenceBetweenImages);
            float getFocalDifferenceBetweenImages() const;

            void setConfocalOpeningHalfAngle(float confocalOpeningHalfAngle);
            float getConfocalOpeningHalfAngle() const;

            virtual void addToVariableContainer(VariableContainer* container) const override;

        private:
            float tiltAngle;
            float focalDepth;
            float focalDifferenceBetweenImages;
            float confocalOpeningHalfAngle;
        };

        std::ostream& operator<<(std::ostream& os, STEMScannerGeometry* geometry);

    }
}