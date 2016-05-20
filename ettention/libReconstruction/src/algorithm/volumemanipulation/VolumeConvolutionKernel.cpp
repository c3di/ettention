#include "stdafx.h"
#include "VolumeConvolutionKernel.h"
#include "VolumeConvolution_bin2c.h"
#include "framework/Framework.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    VolumeConvolutionKernel::VolumeConvolutionKernel(Framework* framework, GPUMappedVolume* input, GPUMappedVolume* output, GPUMappedVolume* convolutionKernel, GPUMappedVolume* mask /*= nullptr*/) : ComputeKernel(framework, VolumeConvolution_kernel_SourceCode, "execute", "VolumeConvolution", (mask ? "-D USE_PRIOR_KNOWLEDGE_MASK" : ""))
        , input(input)
        , output(output)
        , convolutionKernel(convolutionKernel)
        , mask(mask)
    {
        auto conbolutionKernelSize = convolutionKernel->getMappedSubVolume()->getResolution();
        if( conbolutionKernelSize.x % 2 == 0 || conbolutionKernelSize.y % 2 == 0 || conbolutionKernelSize.z % 2 == 0 )
        {
            throw Exception("Volume convolution kernel must have an odd entry in each dimension.");
        }

        replacementMaskGPUVolume = new GPUMappedVolume(framework->getOpenCLStack(), new ByteVolume(Vec3ui(1, 1, 1), 0.0f));
        replacementMaskGPUVolume->takeOwnershipOfObjectOnCPU();
    }

    VolumeConvolutionKernel::~VolumeConvolutionKernel()
    {
        delete replacementMaskGPUVolume;
    }

    void VolumeConvolutionKernel::prepareKernelArguments()
    {
		implementation->setArgument("input", input );
        implementation->setArgument("output", output->getBufferOnGPU());
        implementation->setArgument("kernelValues", convolutionKernel);
        if(mask)
        {
            implementation->setArgument("mask", mask);
        } else {
            implementation->setArgument("mask", replacementMaskGPUVolume);
        }
        implementation->setArgument("kernelRes", convolutionKernel->getMappedSubVolume()->getResolution());
    }

    void VolumeConvolutionKernel::preRun()
    {
        if( input->getMappedSubVolume()->getResolution() != output->getMappedSubVolume()->getResolution() + convolutionKernel->getMappedSubVolume()->getResolution() - Vec3ui(1, 1, 1) )
        {
            throw Exception("Volume convolution input must provide a border of the size of the kernel radius!");
        }
        implementation->setGlobalWorkSize(output->getMappedSubVolume()->getResolution());
    }

    void VolumeConvolutionKernel::postRun()
    {
        output->markAsChangedOnGPU();
    }
}