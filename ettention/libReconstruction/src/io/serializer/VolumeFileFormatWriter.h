#pragma once
#include <stdio.h>
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"
#include "io/rangetransformation/RangeTransformation.h"
#include "setup/parameterset/IOEnumerations.h"

namespace ettention
{
    class OutputParameterSet;

    class VolumeFileFormatWriter
    {
    public:
        virtual bool canGenerateVoxelType(VoxelValueType mode) = 0;
        virtual bool canGenerateRotation(CoordinateOrder rotation) = 0;
        virtual std::string getFileExtension() = 0;
        virtual void write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputParameterSet* options) = 0;

    protected:
        RangeTransformation* createRangeTransformation(Volume* volume, const OutputParameterSet* options);
        void fillOrderDependentIndices(unsigned int orderDependentIndices[3], CoordinateOrder order) const;
        void writeValue(std::ofstream &output, float value, VoxelValueType valueType);
    };

}
