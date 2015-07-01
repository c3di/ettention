#include "stdafx.h"

#include "model/volume/Volume.h"
#include "model/volume/VolumeProperties.h"
#include "io/datasource/MRCStack.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/datasource/ImageStackDatasource.h"

namespace ettention
{
    Volume* VolumeDeserializer::load(ImageStackDatasource* dataSource, VoxelType voxelType)
    {
        Vec3ui volumeResolution = Vec3ui(dataSource->getResolution(), dataSource->getNumberOfProjections());
        auto volume = new Volume(VolumeProperties(voxelType, VolumeParameterSet(volumeResolution), 1), 0.0f);
        for(unsigned int z = 0; z < volumeResolution.z; z++)
        {
            volume->setZSlice(z, dataSource->getProjectionImage(z));
        }
        return volume;
    }

    Volume* VolumeDeserializer::load(std::string volumeFileName, VoxelType voxelType)
    {
        MRCStack* dataSource = new MRCStack(volumeFileName, false);
        auto volume = load(dataSource, voxelType);
        delete dataSource;
        return volume;
    }
}
