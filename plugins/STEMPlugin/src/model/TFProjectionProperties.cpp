#include "stdafx.h"
#include "TFProjectionProperties.h"
#include "gpu/variables/VariableContainer.h"

namespace ettention
{
    namespace stem
    {
        TFProjectionProperties::TFProjectionProperties()
            : tiltAngle(0.0f)
            , focalDepth(0.0f)
            , focalDifferenceBetweenImages(0.0f)
            , confocalOpeningHalfAngle(0.0f)
            , variableContainer(new VariableContainer)
        {
        }

        TFProjectionProperties::TFProjectionProperties(float tiltAngle, float focalDepth, float focalDifferenceBetweenImages, float confocalOpeningHalfAngle)
            : tiltAngle(tiltAngle)
            , focalDepth(focalDepth)
            , focalDifferenceBetweenImages(focalDifferenceBetweenImages)
            , confocalOpeningHalfAngle(confocalOpeningHalfAngle)
            , variableContainer(new VariableContainer)
        {
        }

        TFProjectionProperties::TFProjectionProperties(const TFProjectionProperties& other)
            : tiltAngle(other.tiltAngle)
            , focalDepth(other.focalDepth)
            , focalDifferenceBetweenImages(other.focalDifferenceBetweenImages)
            , confocalOpeningHalfAngle(other.confocalOpeningHalfAngle)
            , variableContainer(new VariableContainer)
        {
        }

        TFProjectionProperties& TFProjectionProperties::operator=(const TFProjectionProperties& other)
        {
            tiltAngle = other.tiltAngle;
            focalDepth = other.focalDepth;
            focalDifferenceBetweenImages = other.focalDifferenceBetweenImages;
            confocalOpeningHalfAngle = other.confocalOpeningHalfAngle;
            return *this;
        }

        TFProjectionProperties::~TFProjectionProperties()
        {
            delete variableContainer;
        }

        void TFProjectionProperties::SetTiltAngle(float tiltAngle)
        {
            this->tiltAngle = tiltAngle;
        }

        float TFProjectionProperties::GetTiltAngle() const
        {
            return tiltAngle;
        }

        void TFProjectionProperties::SetFocalDepth(float focalDepth)
        {
            this->focalDepth = focalDepth;
        }

        float TFProjectionProperties::GetFocalDepth() const
        {
            return focalDepth;
        }

        void TFProjectionProperties::SetFocalDifferenceBetweenImages(float focalDifferenceBetweenImages)
        {
            this->focalDifferenceBetweenImages = focalDifferenceBetweenImages;
        }

        float TFProjectionProperties::GetFocalDifferenceBetweenImages() const
        {
            return focalDifferenceBetweenImages;
        }

        void TFProjectionProperties::SetConfocalOpeningHalfAngle(float confocalOpeningHalfAngle)
        {
            this->confocalOpeningHalfAngle = confocalOpeningHalfAngle;
        }

        float TFProjectionProperties::GetConfocalOpeningHalfAngle() const
        {
            return confocalOpeningHalfAngle;
        }

        VariableContainer* TFProjectionProperties::getVariableContainer() const
        {
            variableContainer->Clear();
            variableContainer->Push(KernelVariable::CreateFromFloat("tiltAngle", this->GetTiltAngle()));
            variableContainer->Push(KernelVariable::CreateFromFloat("openingHalfAngle", this->GetConfocalOpeningHalfAngle()));
            variableContainer->Push(KernelVariable::CreateFromFloat("openingHalfAngleTangens", tanf(this->GetConfocalOpeningHalfAngle())));
            variableContainer->Push(KernelVariable::CreateFromFloat("focalDistance", this->GetFocalDepth()));
            variableContainer->Push(KernelVariable::CreateFromFloat("focalDifferenceBetweenImages", this->GetFocalDifferenceBetweenImages()));
            return variableContainer;
        }

        std::ostream& operator<<(std::ostream& str, const TFProjectionProperties& props)
        {
            str << "TODO";
            return str;
        }
    }
}