#include "stdafx.h"

#include "STEMScannerGeometry.h"
#include "gpu/variables/KernelVariable.h"
#include "gpu/variables/VariableContainer.h"

namespace ettention
{
    namespace stem
    {
        STEMScannerGeometry::STEMScannerGeometry()
            : ParallelScannerGeometry()
        {
        }

        STEMScannerGeometry::STEMScannerGeometry(const STEMScannerGeometry* other)
            : ParallelScannerGeometry(other)
        {
            this->tiltAngle = other->tiltAngle;
            this->focalDepth = other->focalDepth;
            this->focalDifferenceBetweenImages = other->focalDifferenceBetweenImages;
            this->confocalOpeningHalfAngle = other->confocalOpeningHalfAngle;
        }

        STEMScannerGeometry::STEMScannerGeometry(ImageStackDatasource* inputStack)
            : ParallelScannerGeometry(inputStack)
        {
        }

        STEMScannerGeometry::STEMScannerGeometry(const Vec2ui& resolution)
            : ParallelScannerGeometry(resolution)
        {
        }

        STEMScannerGeometry::STEMScannerGeometry(const Vec3f& sourceBase, 
                                                 const Vec3f& detectorBase, 
                                                 const Vec3f& horizontalPitch, 
                                                 const Vec3f& verticalPitch, 
                                                 const Vec2ui& resolution, 
                                                 float tiltAngle, 
                                                 float focalDepth, 
                                                 float focalDifferenceBetweenImages, 
                                                 float confocalOpeningHalfAngle)
        : ParallelScannerGeometry(sourceBase, detectorBase, horizontalPitch, verticalPitch, resolution)
        {
            this->tiltAngle = tiltAngle;
            this->focalDepth = focalDepth;
            this->focalDifferenceBetweenImages = focalDifferenceBetweenImages;
            this->confocalOpeningHalfAngle = confocalOpeningHalfAngle;
        }

        STEMScannerGeometry::~STEMScannerGeometry()
        {
        }

        ScannerGeometry* STEMScannerGeometry::clone() const
        {
            return new STEMScannerGeometry(this);
        }

        void STEMScannerGeometry::setTiltAngle(float tiltAngle)
        {
            this->tiltAngle = tiltAngle;
        }

        float STEMScannerGeometry::getTiltAngle() const
        {
            return tiltAngle;
        }

        void STEMScannerGeometry::setFocalDepth(float focalDepth)
        {
            this->focalDepth = focalDepth;
        }

        float STEMScannerGeometry::getFocalDepth() const
        {
            return focalDepth;
        }

        void STEMScannerGeometry::setFocalDifferenceBetweenImages(float focalDifferenceBetweenImages)
        {
            this->focalDifferenceBetweenImages = focalDifferenceBetweenImages;
        }

        float STEMScannerGeometry::getFocalDifferenceBetweenImages() const
        {
            return focalDifferenceBetweenImages;
        }

        void STEMScannerGeometry::setConfocalOpeningHalfAngle(float confocalOpeningHalfAngle)
        {
            this->confocalOpeningHalfAngle = confocalOpeningHalfAngle;
        }

        float STEMScannerGeometry::getConfocalOpeningHalfAngle() const
        {
            return confocalOpeningHalfAngle;
        }

        void STEMScannerGeometry::addToVariableContainer(VariableContainer* container) const 
        {
            ParallelScannerGeometry::addToVariableContainer(container);
            container->push(KernelVariable::createFromFloat("openingHalfAngle", this->getConfocalOpeningHalfAngle()));
            container->push(KernelVariable::createFromFloat("openingHalfAngleTangens", tanf(this->getConfocalOpeningHalfAngle())));
            container->push(KernelVariable::createFromFloat("focalDistance", this->getFocalDepth()));
            container->push(KernelVariable::createFromFloat("focalDifferenceBetweenImages", this->getFocalDifferenceBetweenImages()));
        }

        std::ostream& operator<<(std::ostream& os, STEMScannerGeometry* geometry)
        {
            os << "( tilt angle " << geometry->getTiltAngle() << " focal position " << geometry->getFocalDepth() << ")";
            return os;
        }

    }
}