#include "stdafx.h"
#include "ContourForwardProjectionCreator.h"
#include "ContourForwardProjectionKernel.h"
#include <model/geometry/GeometricSetup.h>

namespace ettention
{
    namespace contour
    {
        ContourForwardProjectionCreator::ContourForwardProjectionCreator()
        {
        }

        ContourForwardProjectionCreator::~ContourForwardProjectionCreator()
        {
        }

        ForwardProjectionKernel* ContourForwardProjectionCreator::CreateKernelInstance(Framework* framework, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask) const
        {
            if(priorKnowledgeMask && geometricSetup->getProjectionType() == ScannerGeometry::PROJECTION_PARALLEL)
            {
                return new ContourForwardProjectionKernel(framework, geometricSetup, volume, priorKnowledgeMask);
            }
            return 0;
        }
    }
}