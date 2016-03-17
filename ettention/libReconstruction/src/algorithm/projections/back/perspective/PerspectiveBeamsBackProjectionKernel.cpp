#include "stdafx.h"
#include "PerspectiveBeamsBackProjectionKernel.h"
#include "PerspectiveBeamsBackProjection_bin2c.h"
#include "framework/Framework.h"
#include "framework/math/Matrix4x4.h"
#include "gpu/opencl/CLKernel.h"
#include "model/geometry/GeometricSetup.h"
#include "setup/parameterset/PriorKnowledgeParameterSet.h"

namespace ettention
{
    PerspectiveBeamsBackProjectionKernel::PerspectiveBeamsBackProjectionKernel(Framework* framework, 
                                                                               const GeometricSetup* geometricSetup,
                                                                               GPUMappedVolume* volume,
                                                                               GPUMappedVolume* priorKnowledgeMask,
                                                                               float lambda,
                                                                               unsigned int samplesPerVoxel,
                                                                               bool useLongObjectCompensation)
        : BackProjectionKernel(framework,
                               PerspectiveBeamsBackProjection_kernel_SourceCode,
                               "correct",
                               "Backprojection (perspective,voxelwise,basis:voxel)",
                               geometricSetup,
                               volume,
                               priorKnowledgeMask,
                               lambda,
                               1,
                               useLongObjectCompensation,
                               true,
                               getKernelDefines(framework))
        , samplesPerVoxel(samplesPerVoxel)
    {
        transform = new GPUMapped<Matrix4x4>(abstractionLayer, Vec2ui(4, 4));
    }

    PerspectiveBeamsBackProjectionKernel::~PerspectiveBeamsBackProjectionKernel()
    {
        delete transform;
    }

    void PerspectiveBeamsBackProjectionKernel::preRun()
    {
        BackProjectionKernel::preRun();
        auto samples = this->computeSamplingFactor(residual->getResolution(), volume->getMappedSubVolume()->getResolution(), samplesPerVoxel);
        this->setSamples(samples);
    }

    void PerspectiveBeamsBackProjectionKernel::prepareKernelArguments()
    {
        BackProjectionKernel::prepareKernelArguments();
        transform->setObjectOnCPU(new Matrix4x4(geometricSetup->getScannerGeometry()->getProjectionMatrix()));
        transform->takeOwnershipOfObjectOnCPU();
        implementation->setArgument("transform", transform);
    }

    std::string PerspectiveBeamsBackProjectionKernel::getKernelDefines(Framework* framework)
    {
        std::string defines = "";
        auto paramSet = framework->getParameterSet()->get<PriorKnowledgeParameterSet>();
        if(paramSet->getVolumeMinimumIntensity() != boost::none)
        {
            defines += (boost::format("-D VOLUME_MINIMUM_INTENSITY %04d") % *paramSet->getVolumeMinimumIntensity()).str();
        }
        if(paramSet->getVolumeMaximumIntensity() != boost::none)
        {
            defines += (boost::format("-D VOLUME_MAXIMUM_INTENSITY %04d") % *paramSet->getVolumeMaximumIntensity()).str();
        }
        return defines;
    }

    unsigned int PerspectiveBeamsBackProjectionKernel::computeSamplingFactor(const Vec2ui& projectionResolution, const Vec3ui& volumeResolution, unsigned int samplesPerVoxel) const
    {
        if(samplesPerVoxel != 0)
        {
            return samplesPerVoxel;
        }
        unsigned int maxProjectionResolution = std::max(projectionResolution.x, projectionResolution.y);
        unsigned int maxVolumeResolution = std::max(std::max(volumeResolution.x, volumeResolution.y), volumeResolution.z);
        return std::max(1u, maxProjectionResolution / maxVolumeResolution);
    }
}