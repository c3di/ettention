#include "stdafx.h"
#include "DefaultForwardProjectionCreator.h"
#include "algorithm/blobs/BlobRayIntegrationSampler.h"
#include "algorithm/projections/forward/parallel/ParallelBeamsForwardProjectionKernel.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionKernel.h"
#include "algorithm/projections/forward/perspective/PerspectiveBeamsForwardProjectionWithBlobsKernel.h"
#include "framework/Framework.h"
#include "model/geometry/GeometricSetup.h"
#include "setup/ParameterSet/DebugParameterSet.h"
#include "setup/ParameterSet/ForwardProjectionParameterSet.h"
#include "setup/ParameterSet/OptimizationParameterSet.h"

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
        auto paramSet = framework->getParameterSet();
        if(paramSet->GetBasisFunctions() == AlgebraicParameterSet::BASIS_VOXELS && geometricSetup->getProjectionType() == ScannerGeometry::PROJECTION_PARALLEL)
        {
            return new ParallelBeamsForwardProjectionKernel(framework,
                                                            geometricSetup,
                                                            volume,
                                                            paramSet->get<ForwardProjectionParameterSet>()->Samples());
        }
        if(paramSet->GetBasisFunctions() == AlgebraicParameterSet::BASIS_VOXELS && geometricSetup->getProjectionType() == ScannerGeometry::PROJECTION_PERSPECTIVE)
        {
            return new PerspectiveBeamsForwardProjectionKernel(framework,
                                                               geometricSetup,
                                                               volume,
                                                               paramSet->get<ForwardProjectionParameterSet>()->Samples());
        }
        if(paramSet->GetBasisFunctions() == AlgebraicParameterSet::BASIS_BLOBS && geometricSetup->getProjectionType() == ScannerGeometry::PROJECTION_PERSPECTIVE)
        {
            return new PerspectiveBeamsForwardProjectionWithBlobsKernel(framework,
                                                                        std::make_shared<BlobRayIntegrationSampler>(BlobParameters::CreateDefault(), 256),
                                                                        geometricSetup,
                                                                        volume);
        }
        return 0;
    }
}