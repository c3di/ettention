#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    namespace stem
    {
        class PLUGIN_API GenerateCircularPSFKernel : public ComputeKernel
        {
        public:
            GenerateCircularPSFKernel(Framework* framework, const Vec2ui& projectionResolution, float radius);
            ~GenerateCircularPSFKernel();

            GPUMapped<Image>* getOutput() const;
            void setRadius(float radius);

        protected:
            void prepareKernelArguments() override;
            void preRun() override;
            void postRun() override;

        private:
            float chooseIntensityAccordingToNormalizationCirteria();
            float getSizeOfCircle() const;
            float getSizeOfImage() const;

            GPUMapped<Image>* output;
            float radius;
        };
    }
}