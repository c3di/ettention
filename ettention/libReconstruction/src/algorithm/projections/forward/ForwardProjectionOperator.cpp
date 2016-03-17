#include "stdafx.h"
#include "ForwardProjectionOperator.h"
#include "algorithm/projections/forward/factory/ForwardProjectionFactory.h"
#include "algorithm/projections/forward/ForwardProjectionKernel.h"
#include "framework/Framework.h"
#include "framework/time/PerformanceReport.h"
#include "model/geometry/GeometricSetup.h"

namespace ettention
{
    ForwardProjectionOperator::ForwardProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask)
        : framework(framework)
        , geometricSetup(geometricSetup)
        , volume(volume)
        , priorKnowledgeMask(priorKnowledgeMask)
        , kernel(nullptr)
    {
        kernel = framework->getForwardProjectionFactory()->CreateKernelInstance(framework, geometricSetup, volume, priorKnowledgeMask);
        if(!kernel)
        {
            throw Exception("Parameter set not compatible with known forward projection variants!");
        }
    }

    ForwardProjectionOperator::~ForwardProjectionOperator()
    {
        delete kernel;
        kernel = nullptr;
    }

    void ForwardProjectionOperator::Execute(unsigned int subVolumeIndex)
    {
        if(framework->getPerformanceReport())
        {
            framework->getPerformanceReport()->setCurrentAttribute("Subvolume", subVolumeIndex);
        }
        geometricSetup->setCurrentSubVolumeIndex(subVolumeIndex);
        volume->setCurrentSubvolumeIndex(subVolumeIndex);
        volume->ensureIsUpToDateOnGPU();
        if(priorKnowledgeMask)
        {
            priorKnowledgeMask->setCurrentSubvolumeIndex(subVolumeIndex);
            priorKnowledgeMask->ensureIsUpToDateOnGPU();
        }
        kernel->run();
        if(framework->getPerformanceReport())
        {
            framework->getPerformanceReport()->clearCurrentAttribute("Subvolume");
        }
    }

    void ForwardProjectionOperator::SetOutput(GPUMapped<Image>* virtualProjection, GPUMapped<Image>* traversalLength)
    {
        kernel->setOutput(virtualProjection, traversalLength);
    }
}