#include "stdafx.h"

#include "ContourForwardProjectionKernel.h"
#include "Contour_ForwardProjection_bin2c.h"

namespace ettention
{
    namespace contour
    {
        ContourForwardProjectionKernel::ContourForwardProjectionKernel(Framework* framework,
                                                                       const GeometricSetup* geometricSetup,
                                                                       GPUMappedVolume* volume,
                                                                       GPUMappedVolume* priorKnowledgeMask)
            : ForwardProjectionKernel(framework,
                                      Contour_ForwardProjection_kernel_SourceCode,
                                      "forward",
                                      "ContourForwardProjectionKernel",
                                      geometricSetup,
                                      volume,
                                      priorKnowledgeMask,
                                      0)
        {
        }

        ContourForwardProjectionKernel::~ContourForwardProjectionKernel()
        {
        }
    }
}