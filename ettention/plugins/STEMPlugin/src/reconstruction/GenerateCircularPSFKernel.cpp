#include "stdafx.h"
#include "GenerateCircularPSFKernel.h"
#include "GenerateCircularPSF_bin2c.h"
#include "framework/Framework.h"
#include "framework/math/Matrix4x4.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    namespace stem
    {
        GenerateCircularPSFKernel::GenerateCircularPSFKernel(Framework* framework, const Vec2ui& projectionResolution, float radius)
            : ComputeKernel(framework, GenerateCircularPSF_kernel_SourceCode, "generate", "GenerateCircularPSFKernel")
            , radius(radius)
        {
            output = new GPUMapped<Image>(framework->getOpenCLStack(), projectionResolution);
        }

        GenerateCircularPSFKernel::~GenerateCircularPSFKernel()
        {
            delete output;
        }

        void GenerateCircularPSFKernel::prepareKernelArguments()
        {
            if(radius <= 0.0f)
                throw Exception("not possible to generate image of circle with negative radius");

            implementation->setArgument("resolutionX", output->getResolution().x);
            implementation->setArgument("resolutionY", output->getResolution().y);
            implementation->setArgument("resultImage", output);
            implementation->setArgument("radius", radius);
            implementation->setArgument("outsideIntensity", 0.0f);
            implementation->setArgument("insideIntensity", chooseIntensityAccordingToNormalizationCirteria());
        }

        void GenerateCircularPSFKernel::preRun()
        {
            implementation->setGlobalWorkSize(output->getResolution());
        }

        void GenerateCircularPSFKernel::postRun()
        {
            output->markAsChangedOnGPU();
        }

        GPUMapped<Image>* GenerateCircularPSFKernel::getOutput() const
        {
            return output;
        }

        void GenerateCircularPSFKernel::setRadius(float radius)
        {
            this->radius = radius;
        }

        float GenerateCircularPSFKernel::chooseIntensityAccordingToNormalizationCirteria()
        {
            return 1.0f / getSizeOfCircle();
        }

        float GenerateCircularPSFKernel::getSizeOfCircle() const
        {
            return (float)M_PI * radius * radius;
        }

        float GenerateCircularPSFKernel::getSizeOfImage() const
        {
            return (float)output->getResolution().x * (float)output->getResolution().y;
        }
    }
}