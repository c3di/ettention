#include "stdafx.h"
#include "OutputParameterSet.h"
#include "setup/parametersource/ParameterSource.h"
#include "framework/error/Exception.h"
#include "model/volume/FloatVolume.h"
#include <fstream>
#include <cstdio>

namespace ettention
{
    OutputParameterSet::OutputParameterSet(VoxelValueType voxelValueType, CoordinateOrder orientation, bool invert)
        : voxelType(voxelValueType)
        , orientation(orientation)
        , invert(invert)
    {
    }

    OutputParameterSet::OutputParameterSet(const ParameterSource* parameterSource)
    {
        filename = parameterSource->getPathParameter("output.filename");
        testAvailabilityOfOutputDirectory(filename);

        format = "";
        if(parameterSource->parameterExists("output.format"))
        {
            format = "." + parameterSource->getStringParameter("output.format");
        }

        voxelType = IO_VOXEL_TYPE_FLOAT_32;
        if(parameterSource->parameterExists("output.options.voxelType"))
        {
            voxelType = parseVoxelValueType(parameterSource->getStringParameter("output.options.voxelType"));
        }

        orientation = ORDER_XZY;
        if(parameterSource->parameterExists("output.options.orientation"))
        {
            orientation = parseVoxelCoordinateOrder(parameterSource->getStringParameter("output.options.orientation"));
        }

        invert = false;
        if(parameterSource->parameterExists("output.options.invert"))
        {
            invert = parameterSource->getBoolParameter("output.options.invert");
        }

        enableVoxelization = false;
        if(parameterSource->parameterExists("output.options.enableVoxelization"))
        {
            enableVoxelization = parameterSource->getBoolParameter("output.options.enableVoxelization");
        }
    }

    OutputParameterSet::~OutputParameterSet()
    {
    }

    const boost::filesystem::path& OutputParameterSet::getFilename() const
    {
        return filename;
    }

    const std::string& OutputParameterSet::getFormat() const
    {
        return format;
    }

    VoxelValueType OutputParameterSet::getVoxelType() const
    {
        return voxelType;
    }

    CoordinateOrder OutputParameterSet::getOrientation() const
    {
        return orientation;
    }

    bool OutputParameterSet::getInvert() const
    {
        return invert;
    }

    bool OutputParameterSet::getEnableVoxelization() const
    {
        return enableVoxelization;
    }

    void OutputParameterSet::testAvailabilityOfOutputDirectory(boost::filesystem::path filename)
    {
        std::ofstream outf;
        try
        {
            outf.exceptions(std::ifstream::failbit | std::ofstream::badbit);
            outf.open(filename.string(), std::ofstream::trunc);
            outf << "Test output folder availability.";
            outf.close();
            remove(filename.string().c_str());
        }
        catch (...)
        {
            throw Exception((boost::format("Cannot write to output path. Check its availability and/or permissions. Path: %1%") % filename).str());
        }
    }

    void OutputParameterSet::testValidityOfOutputOptions(VolumeSerializer *serializer) const
    {
        auto *volume = new FloatVolume(Vec3ui(8, 4, 2), 0.0f);
        serializer->write(volume, filename.string(), format, this);
        delete volume;

        remove(filename.string().c_str());
    }

}