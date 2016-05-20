#include "stdafx.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "framework/error/Exception.h"
#include "framework/Framework.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{
    VolumeSerializer::VolumeSerializer()
    {
        registerVolumeFileFormat(new MRCWriter());
    }

    VolumeSerializer::~VolumeSerializer()
    {
        for(auto it = fileFormats.begin(); it != fileFormats.end(); ++it)
            delete it->second;
    }

    void VolumeSerializer::write(GPUMappedVolume* volume, std::string outputVolumeFileName, std::string fileFormatExtension, const OutputParameterSet* options)
    {
        volume->ensureIsUpToDateOnCPU();
        write(volume->getObjectOnCPU(), outputVolumeFileName, fileFormatExtension, options);
    }

    void VolumeSerializer::write(Volume* volume, std::string outputVolumeFileName, std::string fileFormatExtension, const OutputParameterSet* options)
    {
        VolumeFileFormatWriter* writer = findWriter(fileFormatExtension, outputVolumeFileName);
        writer->write(volume, outputVolumeFileName, options);
    }

    void VolumeSerializer::write(GPUMappedVolume* volume, AlgebraicParameterSet* paramSet)
    {
        write(volume, paramSet->get<OutputParameterSet>()->getFilename().string(), paramSet->get<OutputParameterSet>()->getFormat(), paramSet->get<OutputParameterSet>());
    }

    void VolumeSerializer::write(Volume* volume, AlgebraicParameterSet* paramSet)
    {
        write(volume, paramSet->get<OutputParameterSet>()->getFilename().string(), paramSet->get<OutputParameterSet>()->getFormat(), paramSet->get<OutputParameterSet>());
    }

    void VolumeSerializer::registerVolumeFileFormat(VolumeFileFormatWriter* writer)
    {
        fileFormats[writer->getFileExtension()] = writer;
    }

    VolumeFileFormatWriter* VolumeSerializer::findWriter(std::string specifiedExtension, std::string outputVolumeFileName)
    {
        auto formatEntry = fileFormats.find(specifiedExtension);
        if(formatEntry != fileFormats.end())
            return formatEntry->second;

        boost::filesystem::path filepath(outputVolumeFileName);
        specifiedExtension = filepath.extension().string();

        formatEntry = fileFormats.find(specifiedExtension);
        if(formatEntry == fileFormats.end())
            throw Exception("No suitable serializer for volume file format " + specifiedExtension + ". Plugin missing?");

        return formatEntry->second;
    }
}