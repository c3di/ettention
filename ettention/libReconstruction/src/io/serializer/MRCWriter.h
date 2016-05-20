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

    protected:
        virtual void writeHeader(std::ofstream &outfile, VoxelValueType fileMode, VolumeProperties& volumeProperties, const Vec3ui volumeResolution, RangeTransformation* rangeTransformation) override;
    };
}