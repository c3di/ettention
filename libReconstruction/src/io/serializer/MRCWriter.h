#pragma once
#include "io/serializer/VolumeFileFormatWriter.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class MRCWriter : public VolumeFileFormatWriter
    {
    public:
        MRCWriter();;
        virtual ~MRCWriter();;

        virtual bool canGenerateVoxelType(OutputFormatParameterSet::VoxelValueType mode) override;
        virtual bool canGenerateRotation(OutputFormatParameterSet::CoordinateOrder rotation) override;
        virtual std::string getFileExtension() override;
        virtual void write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputFormatParameterSet* options) override;

    protected:
        void writeMRCHeader(OutputFormatParameterSet::VoxelValueType fileMode, VolumeProperties& volumeProperties, OutputFormatParameterSet::CoordinateOrder rotation, RangeTransformation* rangeTransformation);

        void writeMRCSlice(float* slice, size_t length, OutputFormatParameterSet::VoxelValueType fileMode);

        template <typename T>
        void writeMRCSliceInSpecificFormat(float* slice, size_t length);

        boost::filesystem::path outputVolumeFileName;
        std::ofstream outfile;
        void* writeBuffer;
    };
}