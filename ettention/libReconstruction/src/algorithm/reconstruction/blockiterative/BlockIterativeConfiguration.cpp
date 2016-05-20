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
        volume->takeOwnershipOfObjectOnCPU();

        Volume *priorKnowledgeMaskVolumeOnCPU = nullptr;
        auto maskVolumeFilename = framework->getParameterSet()->get<PriorKnowledgeParameterSet>()->getMaskVolumeFilename();
        if(!maskVolumeFilename.empty())
        {
            priorKnowledgeMaskVolumeOnCPU = VolumeDeserializer::load(maskVolumeFilename.string(), Voxel::DataType::UCHAR_8, volume->getProperties().getSubVolumeCount());
        } else 
        if( framework->getParameterSet()->get<PriorKnowledgeParameterSet>()->shouldMaskVolumeBePreallocated() )
        {
            priorKnowledgeMaskVolumeOnCPU = new ByteVolume(volumeOnCpu->getProperties().getVolumeResolution(), 1.0f, volume->getProperties().getSubVolumeCount());
        }

        if(priorKnowledgeMaskVolumeOnCPU)
        {
            priorknowledgeMask = new GPUMappedVolume(framework->getOpenCLStack(), priorKnowledgeMaskVolumeOnCPU);
            priorknowledgeMask->takeOwnershipOfObjectOnCPU();
        }

        HyperStackIndex firstIndex = projectionAccess->getImageStack()->firstIndex();
        auto scannerGeometry = projectionAccess->getImageStack()->createScannerGeometry( firstIndex, paramSet);
        geometricSetup = new GeometricSetup(scannerGeometry);
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
            this->optimizeMemoryUsage();
        }
    }

    BlockIterativeConfiguration::~BlockIterativeConfiguration()
    {
        if(priorknowledgeMask)
        {
            delete priorknowledgeMask;
        }

        delete backProjection;
        delete forwardProjection;

        if( geometricSetup )
            delete geometricSetup;
        else
            throw Exception("~BlockIterativeConfiguration: You managed to delete object twice (geometricSetup).");

        if( volume )
            delete volume;
        else
            throw Exception("~BlockIterativeConfiguration: You managed to delete object twice (volume).");
    }

    void BlockIterativeConfiguration::optimizeMemoryUsage()
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

    GeometricSetup* BlockIterativeConfiguration::getGeometricSetup() const
    {
        return geometricSetup;
    }

    ForwardProjectionOperator* BlockIterativeConfiguration::getForwardProjection() const
    {
        return forwardProjection;
    }

    BackProjectionOperator* BlockIterativeConfiguration::getBackProjection() const
    {
        return backProjection;
    }

    Voxelizer* BlockIterativeConfiguration::getVoxelizer() const
    {
        return voxelizer;
    }

    GPUMappedVolume* BlockIterativeConfiguration::getVolume() const
    {
        return volume;
    }

    GPUMappedVolume* BlockIterativeConfiguration::getPriorknowledgeMask() const
    {
        return priorknowledgeMask;
    }

    void BlockIterativeConfiguration::setPriorknowledgeMask(GPUMappedVolume* volume)
    {
        if( priorknowledgeMask && (priorknowledgeMask != volume) )
            delete priorknowledgeMask;

        priorknowledgeMask = volume;
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