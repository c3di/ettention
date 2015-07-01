#include "stdafx.h"
#include "ContourBackProjectionCreator.h"
#include "ContourBackProjectionKernel.h"
#include <framework/Framework.h>
#include <model/geometry/GeometricSetup.h>
#include <setup/ParameterSet/AlgebraicParameterSet.h>
#include <setup/ParameterSet/AlgorithmParameterSet.h>

namespace ettention
{
    namespace contour
    {
        ContourBackProjectionCreator::ContourBackProjectionCreator()
        {
        }

        ContourBackProjectionCreator::~ContourBackProjectionCreator()
        {
        }

        BackProjectionKernel* ContourBackProjectionCreator::CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const
        {
            if(priorKnowledgeMask && geometricSetup->getProjectionType() == ScannerGeometry::PROJECTION_PARALLEL)
            {
                auto algorithmParameterSet = framework->getParameterSet()->get<AlgorithmParameterSet>();
                return new ContourBackProjectionKernel(framework, geometricSetup, volume, priorKnowledgeMask, algorithmParameterSet->Lambda(), algorithmParameterSet->UseLongObjectCompensation(), true);
            }
            return 0;
        }
    }
}