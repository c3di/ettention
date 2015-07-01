#pragma once

#include <stdio.h>

#include "setup/ParameterSet/OutputFormatParameterSet.h"
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"
#include "io/rangetransformation/RangeTransformation.h"

namespace ettention
{
    class VolumeFileFormatWriter
    {
    public:
        virtual bool canGenerateVoxelType(OutputFormatParameterSet::VoxelValueType mode) = 0;
        virtual bool canGenerateRotation(OutputFormatParameterSet::CoordinateOrder rotation) = 0;
        virtual std::string getFileExtension() = 0;
        virtual void write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputFormatParameterSet* options) = 0;
    protected:
        RangeTransformation* createRangeTransformation(Volume* volume, const OutputFormatParameterSet* options);
    };

}
