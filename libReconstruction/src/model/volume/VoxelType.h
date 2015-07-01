#pragma once
#include <string>
#include "CL/cl.h"

namespace ettention
{

    enum VoxelType
    {
        FLOAT_32,
        HALF_FLOAT_16
    };

    class VoxelTypeInformation
    {
    public:
        static size_t sizeOfVoxelType(VoxelType type);
        static size_t sizeOfChannelType(cl_channel_type type);
        static cl_channel_type getCorrespondingCLTextureChannelType(VoxelType type);
        static VoxelType parseVoxelType(const std::string value);
    };

}
