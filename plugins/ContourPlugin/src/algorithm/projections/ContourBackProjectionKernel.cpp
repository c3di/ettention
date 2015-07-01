#include "stdafx.h"
#include "ContourBackProjectionKernel.h"
#include "Contour_BackProjection_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/geometry/GeometricSetup.h"
#include "framework/geometry/GeometricAlgorithms.h"

namespace ettention
{
    namespace contour
    {
        ContourBackProjectionKernel::ContourBackProjectionKernel(Framework* framework,
                                                                 const GeometricSetup* geometricSetup,
                                                                 GPUMappedVolume* volume,
                                                                 GPUMappedVolume* priorKnowledgeMask,
                                                                 float lambda,
                                                                 bool useLongObjectCompensation,
                                                                 bool useProjectionsAsImages,
                                                                 const std::string& additionalDefines)
            : BackProjectionKernel(framework,
                                   Contour_BackProjection_kernel_SourceCode,
                                   "back",
                                   "ContourBackProjectionKernel",
                                   geometricSetup,
                                   volume,
                                   priorKnowledgeMask,
                                   lambda,
                                   0,
                                   useLongObjectCompensation,
                                   useProjectionsAsImages,
                                   additionalDefines)
        {
            transform = new GPUMapped<Matrix4x4>(abstractionLayer, Vec2ui(4, 4));
        }

        ContourBackProjectionKernel::~ContourBackProjectionKernel()
        {
            delete transform;
        }

        void ContourBackProjectionKernel::prepareKernelArguments()
        {
            BackProjectionKernel::prepareKernelArguments();
            transform->setObjectOnCPU(new Matrix4x4(geometricSetup->getScannerGeometry().getProjectionMatrix()));
            transform->takeOwnershipOfObjectOnCPU();
            implementation->setArgument("transform", transform);
        }
    }
}