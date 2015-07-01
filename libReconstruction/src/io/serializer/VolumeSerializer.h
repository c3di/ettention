#pragma once
#include "setup/ParameterSet/IOParameterSet.h"

namespace ettention
{
    class AlgebraicParameterSet;
    class Framework;
    class GPUMappedVolume;
    class OutputFormatParameterSet;
    class Volume;
    class VolumeFileFormatWriter;

    class VolumeSerializer
    {
    public:
        VolumeSerializer();
        virtual ~VolumeSerializer();

        void registerVolumeFileFormat(VolumeFileFormatWriter* writer);

        void write(GPUMappedVolume* volume,
                   std::string outputVolumeFileName,
                   std::string fileFormatExtension,
                   const OutputFormatParameterSet* options);
        void write(GPUMappedVolume* volume, AlgebraicParameterSet* paramSet);
        void write(Volume* volume,
                   std::string outputVolumeFileName,
                   std::string fileFormatExtension,
                   const OutputFormatParameterSet* options);

        static IOParameterSet::FileMode parseFileMode(std::string modeString);

    protected:
        VolumeFileFormatWriter* findWriter(std::string specifiedExtension, std::string outputVolumeFileName);

        std::unordered_map<std::string, VolumeFileFormatWriter*> fileFormats;
    };

}
