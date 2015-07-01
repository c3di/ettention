#include "stdafx.h"
#include "framework/error/Exception.h"
#include "io/rangetransformation/IdentityRangeTransformation.h"
#include "io/rangetransformation/LinearRangeTransformation.h"
#include "io/serializer/VolumeFileFormatWriter.h"

namespace ettention
{
    RangeTransformation* VolumeFileFormatWriter::createRangeTransformation(Volume* volume, const OutputFormatParameterSet* options)
    {
        VolumeStatistics statistics;
        if(options->InvertData())
        {
            switch(options->VoxelType())
            {
            case OutputFormatParameterSet::UNSIGNED_8:
                return new LinearRangeTransformation(volume, 255.0f, 0.0f);
            case OutputFormatParameterSet::UNSIGNED_16:
                return new LinearRangeTransformation(volume, 65535.0f, 0.0f);
            case OutputFormatParameterSet::SIGNED_16:
                return new LinearRangeTransformation(volume, 32767.0f, -32768.0f);
            case OutputFormatParameterSet::FLOAT_32:
                statistics = VolumeStatistics::compute(volume);
                return new LinearRangeTransformation(volume, statistics.getMax(), statistics.getMin());
            default:
                throw Exception("range transformation for unknown file mode requested");
                break;
            }
        }
        else
        {
            switch(options->VoxelType())
            {
            case OutputFormatParameterSet::UNSIGNED_8:
                return new LinearRangeTransformation(volume, 0.0f, 255.0f);
            case OutputFormatParameterSet::UNSIGNED_16:
                return new LinearRangeTransformation(volume, 0.0f, 65535.0f);
            case OutputFormatParameterSet::SIGNED_16:
                return new LinearRangeTransformation(volume, -32768.0f, 32767.0f);
            case OutputFormatParameterSet::FLOAT_32:
                return new IdentityRangeTransformation(volume);
            default:
                throw Exception("range transformation for unknown file mode requested");
                break;
            }
        }
    }
}