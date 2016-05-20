#include "stdafx.h"

#include "model/volume/Voxel.h"
#include "framework/error/Exception.h"
#include "framework/error/InvalidVoxelTypeException.h"

namespace ettention
{
    size_t Voxel::sizeOfVoxelType(const DataType type)
    {
        switch( type )
        {
        case DataType::UCHAR_8:         return sizeof(byte_t);
        case DataType::HALF_FLOAT_16:  return sizeof(half_float_t);
        case DataType::FLOAT_32:       return sizeof(float);
        default:
            throw InvalidVoxelTypeException();
        }
    }

    size_t Voxel::sizeOfChannelType(const cl_channel_type type)
    {
        switch( type )
        {
        case CL_UNSIGNED_INT8:  return sizeof(cl_uchar);
        case CL_HALF_FLOAT:     return sizeof(cl_half);
        case CL_FLOAT:          return sizeof(cl_float);
        default:
            throw Exception("illegal channel type");
        }
    }

    cl_channel_type Voxel::getCorrespondingCLTextureChannelType(const DataType type)
    {
        switch( type )
        {
        case DataType::UCHAR_8:           return CL_UNSIGNED_INT8;
        case DataType::HALF_FLOAT_16:     return CL_HALF_FLOAT;
        case DataType::FLOAT_32:          return CL_FLOAT;
        default:
            throw InvalidVoxelTypeException();
        }
    }

    std::string Voxel::getKernelDefine(const DataType type)
    {
        switch( type )
        {
        case DataType::UCHAR_8:             return " -D UCHAR_8";
        case DataType::HALF_FLOAT_16:       return " -D HALF_FLOAT_16";
        case DataType::FLOAT_32:            return " -D FLOAT_32";
        default:
            throw InvalidVoxelTypeException();
        }
    }

    Voxel::DataType Voxel::parseVoxelType(const std::string value)
    {
        if( value == "uchar" )
        {
            return DataType::UCHAR_8;
        }
        if( value == "half" )
        {
            return DataType::HALF_FLOAT_16;
        }
        if( value == "float" )
        {
            return DataType::FLOAT_32;
        }
        throw Exception((boost::format("Illegal parameter value for voxel representation: %1%") % value).str().c_str());
    }
}
