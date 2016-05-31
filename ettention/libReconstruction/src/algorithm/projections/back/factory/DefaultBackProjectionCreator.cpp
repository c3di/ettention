#include "stdafx.h"
#include "DefaultBackProjectionCreator.h"
#include "algorithm/blobs/BlobRayIntegrationSampler.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionWithBlobsKernel.h"
#include "framework/Framework.h"
#include "model/geometry/GeometricSetup.h"
#include "model/geometry/ParallelScannerGeometry.h"
#include "model/geometry/PerspectiveScannerGeometry.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/AlgebraicParameterSet.h"
#include "setup/parameterset/BackprojectionParameterSet.h"
#include "setup/parameterset/BlobParameterSet.h"

namespace ettention
{
    DefaultBackProjectionCreator::DefaultBackProjectionCreator()
    {
    }

    DefaultBackProjectionCreator::~DefaultBackProjectionCreator()
    {
    }

    BackProjectionKernel* DefaultBackProjectionCreator::CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const
    {
        auto paramSet = framework->getParameterSet();
        if (dynamic_cast<ParallelScannerGeometry*>(geometricSetup->getScannerGeometry()) != nullptr)
        {
            return new ParallelBeamsBackProjectionKernel(framework,
                geometricSetup,
                volume,
                priorKnowledgeMask,
                paramSet->get<AlgorithmParameterSet>()->getLambda(),
                paramSet->get<BackProjectionParameterSet>()->getSamplesNumber(),
                paramSet->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation());
        }
        if (dynamic_cast<PerspectiveScannerGeometry*>(geometricSetup->getScannerGeometry()) != nullptr)
        {
            if( paramSet->get<AlgorithmParameterSet>()->getBasisFunctions() == AlgorithmParameterSet::BasisFunctionType::BASIS_VOXELS )
            {
                return new PerspectiveBeamsBackProjectionKernel(framework,
                    geometricSetup,
                    volume,
                    priorKnowledgeMask,
                    paramSet->get<AlgorithmParameterSet>()->getLambda(),
                    paramSet->get<BackProjectionParameterSet>()->getSamplesNumber(),
                    paramSet->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation());
            }
            if( paramSet->get<AlgorithmParameterSet>()->getBasisFunctions() == AlgorithmParameterSet::BasisFunctionType::BASIS_BLOBS )
            {
                return new PerspectiveBeamsBackProjectionWithBlobsKernel(framework,
                    geometricSetup,
                    volume,
                    priorKnowledgeMask,
                    framework->getParameterSet()->get<BlobParameterSet>()->getBlobParameters(),
                    paramSet->get<AlgorithmParameterSet>()->getLambda(),
                    paramSet->get<AlgorithmParameterSet>()->shouldUseLongObjectCompensation());
            }
        }
        return nullptr;
    }
}