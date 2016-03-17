#include "stdafx.h"
#include "algorithm/projections/back/BackProjectionKernel.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLImageProperties.h"
#include "gpu/opencl/CLKernel.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "gpu/opencl/CLMemoryStructure.h"
#include "framework/RandomAlgorithms.h"
#include "model/geometry/GeometricSetup.h"
#include "framework/Framework.h"

namespace ettention
{
    BackProjectionKernel::BackProjectionKernel(Framework* framework,
                                               const std::string& sourceCode,
                                               const std::string& kernelName,
                                               const std::string& kernelCaption,
                                               const GeometricSetup* geometricSetup,
                                               GPUMappedVolume* volume,
                                               GPUMappedVolume* priorKnowledgeMask,
                                               float lambda,
                                               unsigned int samples,
                                               bool useLongObjectCompensation,
                                               bool useProjectionsAsImages,
                                               const std::string& additionalDefines)
        : ComputeKernel(framework,
                        sourceCode,
                        kernelName,
                        kernelCaption,
                        additionalDefines + (useLongObjectCompensation ? " -D USE_LONG_OBJECT_COMPENSATION" : "") + (priorKnowledgeMask ? " -D USE_PRIOR_KNOWLEDGE_MASK" : ""))
        , residual(nullptr)
        , rayLength(nullptr)
        , volume(volume)
        , priorKnowledgeMask(priorKnowledgeMask)
        , geometricSetup(geometricSetup)
        , lambda(lambda)
        , useProjectionsAsImages(useProjectionsAsImages)
        , residualImage(nullptr)
        , rayLengthImage(nullptr)
        , projectionBlockSize(1)
    {
        setSamples(samples);

        replacementMaskVolume = new ByteVolume(Vec3ui(1, 1, 1), 0.0f);
        replacementMaskGPUVolume = new GPUMappedVolume(framework->getOpenCLStack(), replacementMaskVolume);
    }

    BackProjectionKernel::~BackProjectionKernel()
    {
        if(useProjectionsAsImages)
        {
            delete residualImage;
            delete rayLengthImage;
        }
        delete replacementMaskGPUVolume;
        delete replacementMaskVolume;
    }

    void BackProjectionKernel::prepareKernelArguments()
    {
        if(residual)
        {
            residual->ensureIsUpToDateOnGPU();
        }
        if(rayLength)
        {
            rayLength->ensureIsUpToDateOnGPU();
        }
        if(useProjectionsAsImages)
        {
            residual->getBufferOnGPU()->copyTo(residualImage->getDataMemObject());
            implementation->setArgument("projection", residualImage);
            if(rayLength)
            {
                rayLength->getBufferOnGPU()->copyTo(rayLengthImage->getDataMemObject());
                implementation->setArgument("rayLength", rayLengthImage);
            }
        } else {
            implementation->setArgument("projection", residual);
            if(rayLength)
            {
                implementation->setArgument("rayLength", rayLength);
            }
        }
        implementation->setArgument("vol", volume->getMappedSubVolume());

        if(priorKnowledgeMask)
        {
            implementation->setArgument("mask", priorKnowledgeMask);
        } else {
            implementation->setArgument("mask", replacementMaskGPUVolume);
        }

        implementation->setArgument("lambda", lambda / (float)projectionBlockSize);
        if(samples != 0)
        {
            implementation->setArgument("nSamples", samples);
        }
        implementation->setArgument("projectionGeometry", geometricSetup->getProjectionVariableContainer());
        implementation->setArgument("volumeGeometry", geometricSetup->getVolumeVariableContainer());
    }

    void BackProjectionKernel::setSubVolumeIndex(unsigned int newIndex)
    {
        subVolumeIndex = newIndex;
    }

    GPUMappedVolume* BackProjectionKernel::getOutput()
    {
        return volume;
    }

    void BackProjectionKernel::preRun()
    {
        implementation->setGlobalWorkSize(volume->getMappedSubVolume()->getResolution());
        if(!residual || !rayLength)
        {
            throw Exception("Residual and traversalLength images must both be provided via SetInput() before kernel launch!");
        }
    }

    void BackProjectionKernel::postRun()
    {
        volume->markAsChangedOnGPU();
    }

    void BackProjectionKernel::SetInput(GPUMapped<Image>* residual, GPUMapped<Image>* traversalLength)
    {
        if(!residual ^ !traversalLength)
        {
            throw Exception("Either residual and traversalLength images must be provided both or none of them!");
        }
        if(residual && traversalLength && residual->getResolution() != traversalLength->getResolution())
        {
            throw Exception("Residual and traversalLength differ in size!");
        }
        auto oldRes = this->residual ? this->residual->getResolution() : Vec2ui(0, 0);
        this->residual = residual;
        this->rayLength = traversalLength;
        if(this->residual && this->useProjectionsAsImages && oldRes != this->residual->getResolution())
        {
            this->allocateProjectionImages(this->residual->getResolution());
        }
    }

    void BackProjectionKernel::allocateProjectionImages(const Vec2ui& projectionResolution)
    {
        CLImageProperties imgProps;
        imgProps.setResolution(projectionResolution);
        imgProps.setNormalizedCoordinates(false);
        imgProps.setAddressingMode(CL_ADDRESS_CLAMP);

        if(rayLengthImage)
        {
            delete rayLengthImage;
        }
        if(residualImage)
        {
            delete residualImage;
        }
        residualImage = CLMemoryStructure::create(abstractionLayer, imgProps);
        rayLengthImage = CLMemoryStructure::create(abstractionLayer, imgProps);
    }

    void BackProjectionKernel::setSamples(unsigned int samples)
    {
        this->samples = samples;
    };

    unsigned int BackProjectionKernel::getMinimumRequiredOversampling(const Vec2ui& projectionResolution, const Vec3ui& volumeResolution) const
    {
        unsigned int maxVolumeRes = std::max(std::max(volumeResolution.x, volumeResolution.y), volumeResolution.z);
        unsigned int minProjectionRes = std::min(projectionResolution.x, projectionResolution.y);

        if(minProjectionRes >= maxVolumeRes * 2)
            return 1;

        return (maxVolumeRes / minProjectionRes) + 1;
    }

    void BackProjectionKernel::SetProjectionBlockSize(unsigned int projectionBlockSize)
    {
        this->projectionBlockSize = projectionBlockSize;
    }
}