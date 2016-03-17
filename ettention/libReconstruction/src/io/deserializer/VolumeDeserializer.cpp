#include "stdafx.h"

#include "VolumeDeserializer.h"
#include "model/volume/Volume.h"
#include "model/volume/ByteVolume.h"
#include "model/volume/HalfFloatVolume.h"
#include "model/volume/FloatVolume.h"
#include "model/volume/VolumeProperties.h"
#include "io/datasource/MRCStack.h"
#include "io/datasource/ImageStackDatasource.h"
#include "framework/error/InvalidVoxelTypeException.h"

namespace ettention
{
    Volume* VolumeDeserializer::load(ImageStackDatasource* dataSource, Voxel::DataType voxelType)
    {
        Vec3ui volumeResolution = Vec3ui(dataSource->getResolution(), dataSource->getNumberOfProjections());

        Volume *volume;
        switch( voxelType )
        {
        case ettention::Voxel::DataType::UCHAR_8:
            volume = new ByteVolume(volumeResolution, 0.0f, 1);
            break;
        case ettention::Voxel::DataType::HALF_FLOAT_16:
            volume = new HalfFloatVolume(volumeResolution, 0.0f, 1);
            break;
        case ettention::Voxel::DataType::FLOAT_32:
            volume = new FloatVolume(volumeResolution, 0.0f, 1);
            break;
        default:
            throw InvalidVoxelTypeException();
        }

        for(unsigned int z = 0; z < volumeResolution.z; z++)
        {
            volume->setZSlice(z, dataSource->getProjectionImage(z));
        }
        return volume;
    }

    Volume* VolumeDeserializer::load(std::string volumeFileName, Voxel::DataType voxelType)
    {
        MRCStack* dataSource = new MRCStack(volumeFileName, false);
        auto volume = load(dataSource, voxelType);
        delete dataSource;
        return volume;
    }
}
