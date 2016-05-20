#pragma once
#include <string>
#include "CL/cl.h"

namespace ettention
{
    typedef uint8_t byte_t;
    typedef uint16_t half_float_t;

    class Voxel
    {
    public:

        enum class DataType
        {
            UCHAR_8 = 0,
            HALF_FLOAT_16 = 1,
            FLOAT_32 = HALF_FLOAT_16 << 1
        };

        static size_t sizeOfVoxelType(const DataType type);
        static size_t sizeOfChannelType(const cl_channel_type type);
        static cl_channel_type getCorrespondingCLTextureChannelType(const DataType type);
        static std::string getKernelDefine(const DataType type);
        static DataType parseVoxelType(const std::string value);
    };

}
