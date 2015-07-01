#include "stdafx.h"
#include "DefaultBackProjectionCreator.h"
#include "algorithm/blobs/BlobRayIntegrationSampler.h"
#include "algorithm/projections/back/parallel/ParallelBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionKernel.h"
#include "algorithm/projections/back/perspective/PerspectiveBeamsBackProjectionWithBlobsKernel.h"
#include "framework/Framework.h"
#include "model/geometry/GeometricSetup.h"
#include "setup/ParameterSet/AlgorithmParameterSet.h"
#include "setup/ParameterSet/BackprojectionParameterSet.h"

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
        switch(geometricSetup->getProjectionType())
        {
        case ScannerGeometry::PROJECTION_PARALLEL:
            return new ParallelBeamsBackProjectionKernel(framework,
                                                         geometricSetup,
                                                         volume,
                                                         paramSet->get<AlgorithmParameterSet>()->Lambda(),
                                                         paramSet->get<BackProjectionParameterSet>()->Samples(),
                                                         paramSet->get<AlgorithmParameterSet>()->UseLongObjectCompensation());
        case ScannerGeometry::PROJECTION_PERSPECTIVE:
            switch(paramSet->GetBasisFunctions())
            {
            case AlgebraicParameterSet::BASIS_VOXELS:
                return new PerspectiveBeamsBackProjectionKernel(framework,
                                                                geometricSetup,
                                                                volume,
                                                                paramSet->get<AlgorithmParameterSet>()->Lambda(),
                                                                paramSet->get<BackProjectionParameterSet>()->Samples(),
                                                                paramSet->get<AlgorithmParameterSet>()->UseLongObjectCompensation());
            case AlgebraicParameterSet::BASIS_BLOBS:
                return new PerspectiveBeamsBackProjectionWithBlobsKernel(framework,
                                                                         geometricSetup,
                                                                         volume,
                                                                         BlobParameters::CreateDefault(),
                                                                         paramSet->get<AlgorithmParameterSet>()->Lambda(),
                                                                         paramSet->get<AlgorithmParameterSet>()->UseLongObjectCompensation());
            }
        }
        return 0;
    }
}