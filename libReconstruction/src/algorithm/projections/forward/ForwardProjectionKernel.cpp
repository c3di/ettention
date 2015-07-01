#include "stdafx.h"
#include "ForwardProjectionKernel.h"
#include "framework/time/Timer.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"
#include "model/geometry/GeometricSetup.h"
#include "model/volume/GPUMappedVolume.h"

namespace ettention 
{   
    ForwardProjectionKernel::ForwardProjectionKernel(Framework* framework,
                                                     const std::string& sourceCode,
                                                     const std::string& kernelName,
                                                     const std::string& kernelCaption,
                                                     const GeometricSetup* geometricSetup,
                                                     GPUMappedVolume* volume,
                                                     GPUMappedVolume* priorKnowledgeMask,
                                                     unsigned int samples)
        : ComputeKernel(framework, sourceCode, kernelName, kernelCaption)
        , projectionResolution(0, 0)
        , volume(volume)
        , priorKnowledgeMask(priorKnowledgeMask)
        , geometricSetup(geometricSetup)
        , traversalLength(0)
        , virtualProjection(0)
        , samples(samples)
    {        
    }
    
    ForwardProjectionKernel::~ForwardProjectionKernel()
    {
    }

    void ForwardProjectionKernel::SetOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength)
    {
        if(!virtualProjection ^ !traversalLength)
        {
            throw Exception("Virtual projection and traversal length must either be set both or none of them!");
        }
        if(virtualProjection && traversalLength && virtualProjection->getResolution() != traversalLength->getResolution())
        {
            throw Exception("Virtual projection and traversal length images differ in size!");
        }
        this->virtualProjection = virtualProjection;
        this->traversalLength = traversalLength;
        if(virtualProjection)
        {
            projectionResolution = virtualProjection->getResolution();
            implementation->setGlobalWorkSize(virtualProjection->getResolution());
        }
        else
        {
            this->projectionResolution = Vec2ui(0, 0);
        }
    }

    void ForwardProjectionKernel::prepareKernelArguments()
    {
        implementation->setArgument("projectionResolution", projectionResolution);
        implementation->setArgument("projection", virtualProjection);
        implementation->setArgument("volume_traversal_length", traversalLength);
        if(samples != 0)
        {
            implementation->setArgument("samples", samples);
        }

        if(volume->getMappedSubVolume()->doesUseImage())
        {
            volume->getMappedSubVolume()->ensureImageIsUpToDate();
            implementation->setArgument("volume", volume->getMappedSubVolume()->getImageOnGPU());
        }
        else
        {
            implementation->setArgument("volume", volume);
        }
        if(priorKnowledgeMask)
        {
            if(priorKnowledgeMask->getMappedSubVolume()->doesUseImage())
            {
                priorKnowledgeMask->getMappedSubVolume()->ensureImageIsUpToDate();
                implementation->setArgument("mask", priorKnowledgeMask->getMappedSubVolume()->getImageOnGPU());
            }
            else
            {
                implementation->setArgument("mask", priorKnowledgeMask);
            }
        }
        implementation->setArgument("projectionGeometry", geometricSetup->getProjectionVariableContainer());
        implementation->setArgument("volumeGeometry", geometricSetup->getVolumeVariableContainer());
    }

    const GeometricSetup* ForwardProjectionKernel::GetGeometricSetup() const
    {
        return this->geometricSetup;
    }

    GPUMappedVolume* ForwardProjectionKernel::getVolume()
    {
        return volume;
    }

    GPUMapped<Image>* ForwardProjectionKernel::GetVirtualProjection() const
    {
        return virtualProjection;
    }

    GPUMapped<Image>* ForwardProjectionKernel::GetTraversalLength() const
    {
        return traversalLength;
    }

    void ForwardProjectionKernel::preRun()
    {
        if(!virtualProjection || !traversalLength)
        {
            throw Exception("ForwardProjectionKernel cannot be run without setting output images! Be sure to call SetOutput() before with non-null parameters.");
        }
        virtualProjection->ensureIsUpToDateOnGPU();
        traversalLength->ensureIsUpToDateOnGPU();
        volume->getMappedSubVolume()->ensureImageIsUpToDate();
    }

    void ForwardProjectionKernel::postRun()
    {
        virtualProjection->markAsChangedOnGPU();
        traversalLength->markAsChangedOnGPU();
    }
}