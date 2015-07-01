#pragma once

#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"
#include "gpu/GPUMapped.h"
#include "reconstruction/GenerateCircularPSFKernel.h"

namespace ettention
{
    namespace stem
    {
        class PLUGIN_API PrefilterOperator
        {
        public:
            PrefilterOperator(Framework* framework, GPUMapped<Image>* residual, float maxRadius);
            virtual ~PrefilterOperator();

            virtual void run();

            virtual float getRadiusOfSamplingStep(int i);
            virtual unsigned int NumberOfSamplingSteps();

            GPUMapped<Image>* getOutput();

            float getMaxRadius() const;
            void setMaxRadius(float val);

        protected:
            virtual void computeAdjointOperator(int i);

            virtual void attachUnfilteredResidual();
            virtual void attachAdjointOperatorAtPosition(int i);

            Vec2ui getTotalResolutionOfPrefilteredResidual();
            void updateOutputBufferResolution();

        protected:
            CLAbstractionLayer* clContext;

            GPUMapped<Image>* residual;
            GPUMapped<Image>* prefilteredResiduals;

            ConvolutionOperatorImplementation* convolutionOperator;
            GenerateCircularPSFKernel* circleGenerator;

            float maxRadius;
        };

    }
}