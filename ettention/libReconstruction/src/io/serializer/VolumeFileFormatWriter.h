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
        virtual void write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputParameterSet* options);


    protected:
        virtual void writeHeader(std::ofstream &outfile, VoxelValueType fileMode, VolumeProperties& volumeProperties, const Vec3ui volumeResolution, RangeTransformation* rangeTransformation) = 0;

        RangeTransformation* createRangeTransformation(Volume* volume, const OutputParameterSet* options);
        void writePixelwise(std::ofstream &outfile, Vec3ui &outputVolumeResolution, Volume* volume, const OutputParameterSet* options, std::unique_ptr<float> &volumeDataInFloatFormat);
        void writeSlicewise(std::ofstream &output, Vec3ui &outputVolumeResolution, Volume* volume, CoordinateOrder orientation);
        void writeValue(std::ofstream &output, float value, VoxelValueType valueType);
        void writeSlice(std::ofstream &output, Image *image);
    };
}
