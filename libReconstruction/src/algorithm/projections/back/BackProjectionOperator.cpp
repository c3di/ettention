#include "stdafx.h"
#include "BackProjectionOperator.h"
#include "algorithm/blobs/BlobVoxelizerOperator.h"
#include "framework/Framework.h"
#include "framework/time/PerformanceReport.h"
#include "model/geometry/GeometricSetup.h"

namespace ettention
{
    BackProjectionOperator::BackProjectionOperator(Framework* framework, GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask)
        : framework(framework)
        , geometricSetup(geometricSetup)
        , volume(volume)
        , priorKnowledgeMask(priorKnowledgeMask)
        , voxelizer(0)
        , kernel(0)
    {
        kernel = framework->getBackProjectionFactory()->CreateKernelInstance(framework, geometricSetup, volume, priorKnowledgeMask);
        if(framework->getParameterSet()->GetBasisFunctions() == AlgebraicParameterSet::BASIS_BLOBS)
        {
            voxelizer = new BlobVoxelizerOperator(framework, BlobParameters::CreateDefault(), volume);
        }
        if(!kernel)
        {
            throw Exception("No fitting back projection kernel found!");
        }
    }


    BackProjectionOperator::~BackProjectionOperator()
    {
        if(voxelizer)
        {
            delete voxelizer;
            voxelizer = 0;
        }
        delete kernel;
    }

    void BackProjectionOperator::execute(unsigned int subVolumeIndex)
    {
        if(subVolumeIndex == ALL_SUB_VOLUMES)
        {
            for(unsigned int i = 0; i < volume->Properties().GetSubVolumeCount(); ++i)
            {
                executeForSingleSubVolume(i);
            }
        }
        else
        {
            executeForSingleSubVolume(subVolumeIndex);
        }
    }

    void BackProjectionOperator::executeForSingleSubVolume(unsigned int subVolumeIndex)
    {
        framework->getPerformanceReport()->setCurrentAttribute("Subvolume", subVolumeIndex);
        geometricSetup->setCurrentSubVolumeIndex(subVolumeIndex);
        volume->setCurrentSubvolumeIndex(subVolumeIndex);
        volume->ensureIsUpToDateOnGPU();
        if(priorKnowledgeMask)
        {
            priorKnowledgeMask->setCurrentSubvolumeIndex(subVolumeIndex);
            priorKnowledgeMask->ensureIsUpToDateOnGPU();
        }
        kernel->run();
        framework->getPerformanceReport()->clearCurrentAttribute("Subvolume");
    }

    GPUMappedVolume* BackProjectionOperator::getRawVolume()
    {
        return kernel->getOutput();
    }

    BackProjectionKernel *BackProjectionOperator::getKernel()
    {
        return kernel;
    }

    Voxelizer* BackProjectionOperator::getVoxelizer()
    {
        return voxelizer;
    }

    void BackProjectionOperator::setInput(GPUMapped<Image>* residual, GPUMapped<Image>* traversalLength, unsigned int projectionBlockSize)
    {
        kernel->SetInput(residual, traversalLength);
        kernel->SetProjectionBlockSize(projectionBlockSize);
    }
}