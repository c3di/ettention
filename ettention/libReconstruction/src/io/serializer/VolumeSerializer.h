#pragma once
#include <stdio.h>
#include "io/rangetransformation/RangeTransformation.h"
#include "io/serializer/VolumeFileFormatWriter.h"
#include "model/volume/GPUMappedVolume.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class OutputParameterSet;
    class AlgebraicParameterSet;
    class Framework;

    class VolumeSerializer
    {
    public:
        VolumeSerializer();
        virtual ~VolumeSerializer();

        void registerVolumeFileFormat(VolumeFileFormatWriter* writer);

        void write(GPUMappedVolume* volume, std::string outputVolumeFileName, std::string fileFormatExtension, const OutputParameterSet* options);
        void write(GPUMappedVolume* volume, AlgebraicParameterSet* paramSet);
        void write(Volume* volume, AlgebraicParameterSet* paramSet);
        void write(Volume* volume, std::string outputVolumeFileName, std::string fileFormatExtension, const OutputParameterSet* options);


    protected:
        VolumeFileFormatWriter* findWriter(std::string specifiedExtension, std::string outputVolumeFileName);
        std::unordered_map<std::string, VolumeFileFormatWriter*> fileFormats;
    };

}
