#include "stdafx.h"
#include "BlockIterativeConfiguration.h"
#include "framework/Framework.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "model/geometry/GeometricSetup.h"
#include "setup/ParameterSet/OptimizationParameterSet.h"
#include "setup/ParameterSet/PriorKnowledgeParameterSet.h"

namespace ettention
{
    BlockIterativeConfiguration::BlockIterativeConfiguration(Framework* framework, BlockIterativeProjectionAccess* projectionAccess)
        : paramSet(framework->getParameterSet())
        , projectionAccess(projectionAccess)
        , priorknowledgeMask(0)
    {
        if(paramSet->get<VolumeParameterSet>()->InitFile() != "")
        {
            throw Exception("Volume initialization from file not yet implemented!");
        }
        auto volumeOnCpu = new  Volume(VolumeProperties(paramSet->get<OptimizationParameterSet>()->VoxelRepresentation(), *paramSet->get<VolumeParameterSet>(), paramSet->get<HardwareParameterSet>()->SubVolumeCount()), paramSet->get<VolumeParameterSet>()->InitValue());
        volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
        auto maskVolumeFilename = framework->getParameterSet()->get<PriorKnowledgeParameterSet>()->getMaskVolumeFilename();
        if(!maskVolumeFilename.empty())
        {
            auto priorKnowledgeMaskVolumeOnCPU = VolumeDeserializer::load(maskVolumeFilename.string(), FLOAT_32);
            priorknowledgeMask = new GPUMappedVolume(framework->getOpenCLStack(), priorKnowledgeMaskVolumeOnCPU);
        }

        geometricSetup = new GeometricSetup(projectionAccess->getScannerGeometry(0, 0).getProjectionType(), projectionAccess->getProjectionResolution());
        geometricSetup->setVolume(volumeOnCpu);

        forwardProjection = new ForwardProjectionOperator(framework, geometricSetup, volume, priorknowledgeMask);
        backProjection = new BackProjectionOperator(framework, geometricSetup, volume, priorknowledgeMask);
        voxelizer = backProjection->getVoxelizer();

        if(!paramSet->get<HardwareParameterSet>()->SubVolumeCount())
        {
            this->OptimizeMemoryUsage();
        }
    }

    BlockIterativeConfiguration::~BlockIterativeConfiguration()
    {
        if(priorknowledgeMask)
        {
            delete priorknowledgeMask->getObjectOnCPU();
            delete priorknowledgeMask;
        }
        delete backProjection;
        delete forwardProjection;
        delete geometricSetup;
        delete volume->getObjectOnCPU();
        delete volume;
    }

    void BlockIterativeConfiguration::OptimizeMemoryUsage()
    {
        if(voxelizer)
        {
            volume->changeSubVolumeSizeToPartOfTotalMemory(0.8f);
            voxelizer->GetVoxelVolume()->changeSubVolumeSizeToPartOfTotalMemory(0.2f);
        }
        else
        {
            volume->changeSubVolumeSizeToPartOfTotalMemory(1.0f);
        }
        LOGGER("OPTIMIZED MEMORY PROFILE ");
        LOGGER("USING SUBVOLUMES:        " << volume->Properties().GetSubVolumeCount());
    }

    GeometricSetup* BlockIterativeConfiguration::GetGeometricSetup() const
    {
        return geometricSetup;
    }

    ForwardProjectionOperator* BlockIterativeConfiguration::GetForwardProjection() const
    {
        return forwardProjection;
    }

    BackProjectionOperator* BlockIterativeConfiguration::GetBackProjection() const
    {
        return backProjection;
    }

    Voxelizer* BlockIterativeConfiguration::GetVoxelizer() const
    {
        return voxelizer;
    }

    GPUMappedVolume* BlockIterativeConfiguration::GetVolume() const
    {
        return volume;
    }
}