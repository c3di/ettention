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
                                                     unsigned int samples,
                                                     const std::string& additionalDefines)
        : ComputeKernel(framework, sourceCode, kernelName, kernelCaption,
                        additionalDefines + (priorKnowledgeMask ? " -D USE_PRIOR_KNOWLEDGE_MASK" : ""))
        , projectionResolution(0, 0)
        , volume(volume)
        , priorKnowledgeMask(priorKnowledgeMask)
        , geometricSetup(geometricSetup)
        , traversalLength(nullptr)
        , virtualProjection(nullptr)
        , samples(samples)
    {
    }

    ForwardProjectionKernel::~ForwardProjectionKernel()
    {
    }

    void ForwardProjectionKernel::setOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength)
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

        if(samples == 0)
        {
            samples = 1;
        }
        implementation->setArgument("samples", samples);
        implementation->setArgument("volume", volume);

        if(priorKnowledgeMask)
        {
            implementation->setArgument("mask", priorKnowledgeMask);
        } else {
            // In openCL kernel memory for Mask should be set anyway, even if we do not want use it. So instead of real mask we feed it Volume
            implementation->setArgument("mask", volume);
        }
        implementation->setArgument("projectionGeometry", geometricSetup->getProjectionVariableContainer());
        implementation->setArgument("volumeGeometry", geometricSetup->getVolumeVariableContainer());
    }

    const GeometricSetup* ForwardProjectionKernel::getGeometricSetup() const
    {
        return this->geometricSetup;
    }

    GPUMappedVolume* ForwardProjectionKernel::getVolume()
    {
        return volume;
    }

    GPUMapped<Image>* ForwardProjectionKernel::getVirtualProjection() const
    {
        return virtualProjection;
    }

    GPUMapped<Image>* ForwardProjectionKernel::getTraversalLength() const
    {
        return traversalLength;
    }

    void ForwardProjectionKernel::preRun()
    {
        if(!virtualProjection || !traversalLength)
        {
            throw Exception("ForwardProjectionKernel cannot be run without setting output images! Be sure to call setOutput() before with non-null parameters.");
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