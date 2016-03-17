#include "stdafx.h"
#include "BlockIterativeConfiguration.h"
#include "framework/Framework.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "model/geometry/GeometricSetup.h"
#include "setup/parameterset/OptimizationParameterSet.h"
#include "setup/parameterset/OutputParameterSet.h"
#include "setup/parameterset/PriorKnowledgeParameterSet.h"
#include "BlockIterativeProjectionAccess.h"
#include "model/volume/FloatVolume.h"
#include "model/volume/HalfFloatVolume.h"
#include "framework/error/InvalidVoxelTypeException.h"

namespace ettention
{
    BlockIterativeConfiguration::BlockIterativeConfiguration(Framework* framework, BlockIterativeProjectionAccess* projectionAccess)
        : paramSet(framework->getParameterSet())
        , projectionAccess(projectionAccess)
        , priorknowledgeMask(nullptr)
    {
        Volume* volumeOnCpu;

        auto voxelRepresentation = paramSet->get<OptimizationParameterSet>()->getVoxelRepresentation();
        switch( voxelRepresentation )
        {
        case ettention::Voxel::DataType::HALF_FLOAT_16:
            volumeOnCpu = new HalfFloatVolume((VolumeParameterSet*)paramSet->get<VolumeParameterSet>(), paramSet->get<HardwareParameterSet>()->getSubVolumeCount());
            break;
        case ettention::Voxel::DataType::FLOAT_32:
            volumeOnCpu = new FloatVolume((VolumeParameterSet*)paramSet->get<VolumeParameterSet>(), paramSet->get<HardwareParameterSet>()->getSubVolumeCount());
            break;
        default:
            throw InvalidVoxelTypeException();
        }

        if(paramSet->get<VolumeParameterSet>()->getInitFile() != "")
        {
            auto initialDataVolume = VolumeDeserializer::load(paramSet->get<VolumeParameterSet>()->getInitFile().string(), voxelRepresentation);
            applyInitialVolumeToWorkingVolume(initialDataVolume, volumeOnCpu, paramSet->get<VolumeParameterSet>()->getInitFileOrientation());
            delete initialDataVolume;
        }
        volume = new GPUMappedVolume(framework->getOpenCLStack(), volumeOnCpu);
        auto maskVolumeFilename = framework->getParameterSet()->get<PriorKnowledgeParameterSet>()->getMaskVolumeFilename();
        if(!maskVolumeFilename.empty())
        {
            auto priorKnowledgeMaskVolumeOnCPU = VolumeDeserializer::load(maskVolumeFilename.string(), Voxel::DataType::UCHAR_8);
            priorknowledgeMask = new GPUMappedVolume(framework->getOpenCLStack(), priorKnowledgeMaskVolumeOnCPU);
        }

        geometricSetup = new GeometricSetup( projectionAccess->getProjectionResolution() );
        HyperStackIndex firstIndex = projectionAccess->getImageStack()->firstIndex();
        auto scannerGeometry = projectionAccess->getImageStack()->getScannerGeometry( firstIndex, paramSet);
        geometricSetup->setScannerGeometry( scannerGeometry );
        geometricSetup->setVolume( volumeOnCpu );

        forwardProjection = new ForwardProjectionOperator(framework, geometricSetup, volume, priorknowledgeMask);
        backProjection = new BackProjectionOperator(framework, geometricSetup, volume, priorknowledgeMask);
        voxelizer = 0;
        if(framework->getParameterSet()->get<OutputParameterSet>()->getEnableVoxelization())
        {
            voxelizer = backProjection->getVoxelizer();
        }

        if(!paramSet->get<HardwareParameterSet>()->getSubVolumeCount())
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
            voxelizer->getVoxelVolume()->changeSubVolumeSizeToPartOfTotalMemory(0.2f);
        }
        else
        {
            volume->changeSubVolumeSizeToPartOfTotalMemory(1.0f);
        }
        LOGGER("OPTIMIZED MEMORY PROFILE ");
        LOGGER("USING SUBVOLUMES:        " << volume->getProperties().getSubVolumeCount());
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

    void BlockIterativeConfiguration::applyInitialVolumeToWorkingVolume(Volume* initialVolume, Volume* workingVolume, CoordinateOrder initialVolumeCoordinateOrder)
    {
        auto res = translateCoordinates(initialVolume->getProperties().getVolumeResolution(), initialVolumeCoordinateOrder);
        if(res != workingVolume->getProperties().getVolumeResolution())
        {
            throw Exception("Resolution of volume from file " + paramSet->get<VolumeParameterSet>()->getInitFile().string() + " does not match resolution given in config file!");
        }
        for(unsigned int z = 0; z < initialVolume->getProperties().getVolumeResolution().z; ++z)
        {
            for(unsigned int y = 0; y < initialVolume->getProperties().getVolumeResolution().y; ++y)
            {
                for(unsigned int x = 0; x < initialVolume->getProperties().getVolumeResolution().x; ++x)
                {
                    Vec3ui initialCoords(x, y, z);
                    Vec3ui workingCoords = translateCoordinates(initialCoords, initialVolumeCoordinateOrder);
                    workingVolume->setVoxelToValue(workingCoords, initialVolume->getVoxelValue(initialCoords));
                }
            }
        }
    }

    Vec3ui BlockIterativeConfiguration::translateCoordinates(const Vec3ui& from, CoordinateOrder order)
    {
        switch(order)
        {
        case ORDER_XYZ:
            return from;
        case ORDER_XZY:
            return Vec3ui(from.x, from.z, from.y);
        case ORDER_YXZ:
            return Vec3ui(from.y, from.x, from.z);
        case ORDER_YZX:
            return Vec3ui(from.z, from.x, from.y);
        case ORDER_ZXY:
            return Vec3ui(from.y, from.z, from.x);
        case ORDER_ZYX:
            return Vec3ui(from.z, from.y, from.x);
        default:
            throw Exception("Unknown CoordinateOrder given!");
        }
    }
}