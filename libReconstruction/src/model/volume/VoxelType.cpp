#include "stdafx.h"

#include "model/volume/VoxelType.h"
#include "framework/error/Exception.h"

namespace ettention
{
    size_t VoxelTypeInformation::sizeOfVoxelType(VoxelType type)
    {
        if(type == VoxelType::FLOAT_32)
            return sizeof(float);
        else if(type == VoxelType::HALF_FLOAT_16)
            return sizeof(unsigned short);
        throw Exception("illegal voxel type");
    }

    size_t VoxelTypeInformation::sizeOfChannelType(cl_channel_type type)
    {
        if(type == CL_FLOAT)
            return sizeof(float);
        else if(type == CL_HALF_FLOAT)
            return sizeof(unsigned short);
        throw Exception("illegal channel type");
    }

    cl_channel_type VoxelTypeInformation::getCorrespondingCLTextureChannelType(VoxelType type)
    {
        if(type == VoxelType::FLOAT_32)
        {
            return CL_FLOAT;
        }
        if(type == VoxelType::HALF_FLOAT_16)
        {
            return CL_HALF_FLOAT;
        }
        throw Exception("unknown voxel type");
    }

    VoxelType VoxelTypeInformation::parseVoxelType(const std::string value)
    {
        if(value == "float")
        {
            return VoxelType::FLOAT_32;
        }
        if(value == "half")
        {
            return VoxelType::HALF_FLOAT_16;
        }
        throw Exception((boost::format("Illegal parameter value for voxelRepresentation : %1%") % value).str().c_str());
    }


}
