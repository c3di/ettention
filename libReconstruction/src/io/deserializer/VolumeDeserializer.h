#pragma once

#include <string>
#include <iostream>
#include <memory>

#include "model/volume/Volume.h"

namespace ettention
{
    class ImageStackDatasource;

    /*
    The class reads entire volumes from disc. You can either pass a filename to an volume file in mrc format, an std::istring or an ImageStackDatasource. In either case,
    the volume is read entirely to memory. 
    */
    class VolumeDeserializer
    {
    public:
        static Volume* load(std::string fileName, VoxelType voxelType);
        static Volume* load(ImageStackDatasource* dataSource, VoxelType voxelType);
    };   
}
