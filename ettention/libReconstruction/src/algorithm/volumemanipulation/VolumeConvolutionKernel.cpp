#include "stdafx.h"
#include "VolumeConvolutionKernel.h"
#include "VolumeConvolution_bin2c.h"
#include "framework/Framework.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    VolumeConvolutionKernel::VolumeConvolutionKernel(Framework* framework, GPUMapped<SubVolume>* input, GPUMapped<SubVolume>* output, GPUMapped<SubVolume>* convolutionKernel)
        : ComputeKernel(framework, VolumeConvolution_kernel_SourceCode, "execute", "VolumeConvolution")
        , input(input)
        , output(output)
        , convolutionKernel(convolutionKernel)
    {
        if(convolutionKernel->getResolution().x % 2 == 0 || convolutionKernel->getResolution().y % 2 == 0 || convolutionKernel->getResolution().z % 2 == 0)
        {
            throw Exception("Volume convolution kernel must have an odd entry in each dimension.");
        }
    }

    VolumeConvolutionKernel::~VolumeConvolutionKernel()
    {
    }

    void VolumeConvolutionKernel::prepareKernelArguments()
    {
        implementation->setArgument("input", input);
        implementation->setArgument("output", output);
        implementation->setArgument("kernelValues", convolutionKernel);
        implementation->setArgument("kernelRes", convolutionKernel->getResolution());
    }

    void VolumeConvolutionKernel::preRun()
    {
        if(input->getResolution() != output->getResolution() + convolutionKernel->getResolution() - Vec3ui(1, 1, 1))
        {
            throw Exception("Volume convolution input must provide a border of the size of the kernel radius!");
        }
        implementation->setGlobalWorkSize(output->getResolution());
    }

    void VolumeConvolutionKernel::postRun()
    {
        output->markAsChangedOnGPU();
    }
}