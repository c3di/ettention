#include "stdafx.h"
#include "DefaultForwardProjectionCreator.h"
#include "algorithm/blobs/BlobRayIntegrationSampler.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionKernel.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionWithBlobsKernel.h"
#include "framework/Framework.h"
#include "model/geometry/GeometricSetup.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/BlobParameterSet.h"
#include "setup/parameterset/DebugParameterSet.h"
#include "setup/parameterset/ForwardProjectionParameterSet.h"
#include "setup/parameterset/OptimizationParameterSet.h"
#include "model/geometry/ParallelScannerGeometry.h"
#include "model/geometry/PerspectiveScannerGeometry.h"

namespace ettention
{
    DefaultForwardProjectionCreator::DefaultForwardProjectionCreator()
    {
    }

    DefaultForwardProjectionCreator::~DefaultForwardProjectionCreator()
    {
    }

    ForwardProjectionKernel* DefaultForwardProjectionCreator::CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const
    {
        auto *paramSet = framework->getParameterSet();
        if (dynamic_cast<ParallelScannerGeometry*>(geometricSetup->getScannerGeometry()) != nullptr)
        {
            if( paramSet->get<AlgorithmParameterSet>()->getBasisFunctions() == AlgorithmParameterSet::BasisFunctionType::BASIS_VOXELS )
            {
                return new ParallelBeamsForwardProjectionKernel(framework,
                                                                geometricSetup,
                                                                volume,
                                                                priorKnowledgeMask,
                                                                paramSet->get<ForwardProjectionParameterSet>()->getSamplesNumber());
            }
        }

        if (dynamic_cast<PerspectiveScannerGeometry*>(geometricSetup->getScannerGeometry()) != nullptr)
        {
            if( paramSet->get<AlgorithmParameterSet>()->getBasisFunctions() == AlgorithmParameterSet::BasisFunctionType::BASIS_VOXELS )
            {
                return new PerspectiveBeamsForwardProjectionKernel(framework,
                                                                   geometricSetup,
                                                                   volume,
                                                                   priorKnowledgeMask,
                                                                   paramSet->get<ForwardProjectionParameterSet>()->getSamplesNumber());
            }
            if( paramSet->get<AlgorithmParameterSet>()->getBasisFunctions() == AlgorithmParameterSet::BasisFunctionType::BASIS_BLOBS )
            {
                return new PerspectiveBeamsForwardProjectionWithBlobsKernel(framework,
                                                                            std::make_shared<BlobRayIntegrationSampler>(framework->getParameterSet()->get<BlobParameterSet>()->getBlobParameters(), 256),
                                                                            geometricSetup,
                                                                            volume,
                                                                            priorKnowledgeMask);
            }
        }
        return nullptr;
    }
}