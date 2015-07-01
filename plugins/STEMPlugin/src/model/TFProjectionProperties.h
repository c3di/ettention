#pragma once

namespace ettention
{
    class VariableContainer;

    namespace stem
    {
        class PLUGIN_API TFProjectionProperties
        {
        public:
            TFProjectionProperties();
            TFProjectionProperties(float tiltAngle, float focalDepth, float focalDifferenceBetweenImages, float confocalOpeningHalfAngle);
            TFProjectionProperties(const TFProjectionProperties& other);
            ~TFProjectionProperties();

            void SetTiltAngle(float tiltAngle);
            float GetTiltAngle() const;
            void SetFocalDepth(float focalDepth);
            float GetFocalDepth() const;
            void SetFocalDifferenceBetweenImages(float focalDifferenceBetweenImages);
            float GetFocalDifferenceBetweenImages() const;
            void SetConfocalOpeningHalfAngle(float confocalOpeningHalfAngle);
            float GetConfocalOpeningHalfAngle() const;
            VariableContainer* getVariableContainer() const;

            TFProjectionProperties& operator=(const TFProjectionProperties& other);

        private:
            float tiltAngle;
            float focalDepth;
            float focalDifferenceBetweenImages;
            float confocalOpeningHalfAngle;
            VariableContainer* variableContainer;
        };

        std::ostream& operator<<(std::ostream& str, const TFProjectionProperties& props);
    }
}