#include "stdafx.h"
#include "framework/error/Exception.h"
#include "io/rangetransformation/IdentityRangeTransformation.h"
#include "io/rangetransformation/LinearRangeTransformation.h"
#include "io/serializer/VolumeFileFormatWriter.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{
    RangeTransformation* VolumeFileFormatWriter::createRangeTransformation(Volume* volume, const OutputParameterSet* options)
    {
        VolumeStatistics statistics;
        if(options->getInvert())
        {
            switch(options->getVoxelType())
            {
            case IO_VOXEL_TYPE_UNSIGNED_8:
                return new LinearRangeTransformation(volume, 255.0f, 0.0f);
            case IO_VOXEL_TYPE_UNSIGNED_16:
                return new LinearRangeTransformation(volume, 65535.0f, 0.0f);
            case IO_VOXEL_TYPE_SIGNED_16:
                return new LinearRangeTransformation(volume, 32767.0f, -32768.0f);
            case IO_VOXEL_TYPE_FLOAT_32:
                statistics = VolumeStatistics::compute(volume);
                return new LinearRangeTransformation(volume, statistics.getMax(), statistics.getMin());
            default:
                throw Exception("range transformation for unknown file mode requested");
                break;
            }
        }
        else
        {
            switch(options->getVoxelType())
            {
            case IO_VOXEL_TYPE_UNSIGNED_8:
                return new LinearRangeTransformation(volume, 0.0f, 255.0f);
            case IO_VOXEL_TYPE_UNSIGNED_16:
                return new LinearRangeTransformation(volume, 0.0f, 65535.0f);
            case IO_VOXEL_TYPE_SIGNED_16:
                return new LinearRangeTransformation(volume, -32768.0f, 32767.0f);
            case IO_VOXEL_TYPE_FLOAT_32:
                return new IdentityRangeTransformation(volume);
            default:
                throw Exception("range transformation for unknown file mode requested");
                break;
            }
        }
    }

    void VolumeFileFormatWriter::writeValue(std::ofstream &output, float value, VoxelValueType valueType)
    {
        short shortValue;
        unsigned short ushortValue;
        unsigned char ucharValue;
        switch( valueType )
        {
        case IO_VOXEL_TYPE_FLOAT_32:
            output.write((const char*)&value, sizeof(float));
            break;
        case IO_VOXEL_TYPE_SIGNED_16:
            shortValue = (short)value;
            output.write((const char*)&shortValue, sizeof(short));
            break;
        case IO_VOXEL_TYPE_UNSIGNED_16:
            ushortValue = (unsigned short)value;
            output.write((const char*)&ushortValue, sizeof(unsigned short));
            break;
        case IO_VOXEL_TYPE_UNSIGNED_8:
            ucharValue = (unsigned char)value;
            output.write((const char*)&ucharValue, sizeof(unsigned char));
            break;
        default:
            throw Exception("Unknown VoxelValueType given!");
        }
    }

    void VolumeFileFormatWriter::fillOrderDependentIndices(unsigned int orderDependentIndices[3], CoordinateOrder order) const
    {
        switch( order )
        {
        case ORDER_XYZ:
            orderDependentIndices[0] = 2;
            orderDependentIndices[1] = 1;
            orderDependentIndices[2] = 0;
            break;
        case ORDER_XZY:
            orderDependentIndices[0] = 1;
            orderDependentIndices[1] = 2;
            orderDependentIndices[2] = 0;
            break;
        case ORDER_YXZ:
            orderDependentIndices[0] = 2;
            orderDependentIndices[1] = 0;
            orderDependentIndices[2] = 1;
            break;
        case ORDER_YZX:
            orderDependentIndices[0] = 0;
            orderDependentIndices[1] = 2;
            orderDependentIndices[2] = 1;
            break;
        case ORDER_ZXY:
            orderDependentIndices[0] = 1;
            orderDependentIndices[1] = 0;
            orderDependentIndices[2] = 2;
            break;
        case ORDER_ZYX:
            orderDependentIndices[0] = 0;
            orderDependentIndices[1] = 1;
            orderDependentIndices[2] = 2;
            break;
        }
    }
}