#pragma once
#include "io/serializer/VolumeFileFormatWriter.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/volume/Volume.h"
#include "setup/parameterset/IOEnumerations.h"

namespace ettention
{
    class MRCWriter : public VolumeFileFormatWriter
    {
    public:
        MRCWriter();
        virtual ~MRCWriter();

        virtual bool canGenerateVoxelType(VoxelValueType mode) override;
        virtual bool canGenerateRotation(CoordinateOrder rotation) override;
        virtual std::string getFileExtension() override;
        virtual void write(Volume* volume, boost::filesystem::path outputVolumeFileName, const OutputParameterSet* options) override;

    protected:
        void writeMRCHeader(std::ofstream &outfile, VoxelValueType fileMode, VolumeProperties& volumeProperties, const Vec3ui volumeResolution, RangeTransformation* rangeTransformation);
    };
}