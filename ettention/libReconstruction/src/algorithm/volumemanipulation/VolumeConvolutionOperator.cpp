#include "stdafx.h"
#include "VolumeConvolutionOperator.h"
#include "algorithm/volumemanipulation/VolumeConvolutionKernel.h"
#include "gpu/GPUMapped.h"
#include "model/volume/GPUMappedVolume.h"

namespace ettention
{
    VolumeConvolutionOperator::VolumeConvolutionOperator(Framework* framework, GPUMappedVolume* input, GPUMappedVolume* output, GPUMapped<SubVolume>* kernelValues)
        : input(input)
        , output(output)
        , kernelValues(kernelValues)
        , kernel(new VolumeConvolutionKernel(framework, input->getMappedSubVolume(), output->getMappedSubVolume(), kernelValues))
    {
        if(input->getObjectOnCPU()->getProperties().getVolumeResolution() != output->getObjectOnCPU()->getProperties().getVolumeResolution())
        {
            throw Exception("Input and output volume resolutions for VolumeConvolutionKernel differ!");
        }
    }

    VolumeConvolutionOperator::~VolumeConvolutionOperator()
    {
        delete kernel;
    }

    void VolumeConvolutionOperator::run()
    {
        for(unsigned int i = 0; i < output->getObjectOnCPU()->getProperties().getSubVolumeCount(); ++i)
        {
            output->setCurrentSubvolumeIndex(i);
            auto res = output->getObjectOnCPU()->getProperties().getSubVolumeResolution(i) + kernelValues->getResolution() - Vec3ui(1, 1, 1);
            Vec3i base = (Vec3i)output->getObjectOnCPU()->getProperties().getSubVolumeBaseCoords(i) - (Vec3i)kernelValues->getResolution() / 2;
            input->setArbitrarySubVolume(base, res, 0.0f);
            kernel->run();
        }
    }
}