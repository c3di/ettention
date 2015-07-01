#include "stdafx.h"
#include "framework/error/Exception.h"
#include "IOParameterSet.h"
#include "setup/parametersource/ParameterSource.h"
#include "setup/ParameterSet/OutputFormatParameterSet.h"
#include "io/datasource/MRCStack.h"
#include "io/serializer/MRCWriter.h"

namespace ettention
{
    IOParameterSet::IOParameterSet(const ParameterSource* parameterSource)
    {
        inputStackFileName = parameterSource->getPathParameter("input.projections");
        outputVolumeFileName = parameterSource->getPathParameter("output.filename");

        if(parameterSource->parameterExists("input.tiltAngles"))
            tiltAnglesFileName = parameterSource->getPathParameter("input.tiltAngles");

        xAxisTilt = 0.0f;
        if(parameterSource->parameterExists("input.xAxisTilt"))
            xAxisTilt = parameterSource->getFloatParameter("input.xAxisTilt");

        mode = FileMode::UNSPECIFIED;
        if(parameterSource->parameterExists("output.voxelType"))
            mode = parseFileMode(parameterSource->getStringParameter("output.voxelType"));

        logaritmizeData = false;
        if(parameterSource->parameterExists("input.logaritmize"))
            logaritmizeData = parameterSource->getBoolParameter("input.logaritmize");

        outputVolumeRotation = ALONG_XZ;
        if(parameterSource->parameterExists("output.orientation"))
            outputVolumeRotation = parseVolumeRotation(parameterSource->getStringParameter("output.orientation"));

        MRCWriter writer;
        outputVolumeFormat = writer.getFileExtension();
        if(parameterSource->parameterExists("volumeFormat"))
        {
            outputVolumeFormat = parameterSource->getStringParameter("volumeFormat");
        }
    }

    IOParameterSet::IOParameterSet(std::string inputStackFileName, std::string outputVolumeFileName)
        : inputStackFileName(inputStackFileName), outputVolumeFileName(outputVolumeFileName)
    {
        logaritmizeData = true;
        tiltAnglesFileName = "";
    }

    IOParameterSet::IOParameterSet(std::string inputStackFileName)
        : IOParameterSet(inputStackFileName, "")
    {
    }

    IOParameterSet::~IOParameterSet()
    {
    }

    boost::filesystem::path IOParameterSet::OutputVolumeFileName() const
    {
        return outputVolumeFileName;
    }

    boost::filesystem::path IOParameterSet::InputStackFileName() const
    {
        return inputStackFileName;
    }

    IOParameterSet::FileMode IOParameterSet::Mode() const
    {
        return mode;
    }

    bool IOParameterSet::LogaritmizeData() const
    {
        return logaritmizeData;
    }

    IOParameterSet::VolumeRotation IOParameterSet::OutputVolumeRotation() const
    {
        return outputVolumeRotation;
    }

    boost::filesystem::path IOParameterSet::TiltAnglesFileName() const
    {
        return tiltAnglesFileName;
    }


    IOParameterSet::VolumeRotation IOParameterSet::parseVolumeRotation(const std::string& rotationString)
    {
        if(rotationString == "xzy_order")
        {
            return ALONG_XZ;
        }
        else if(rotationString == "xyz_order")
        {
            return ALONG_XY;
        }
        else
        {
            throw Exception("Unknown output volume rotation constant");
        }
    }

    std::string IOParameterSet::OutputVolumeFormat() const
    {
        return outputVolumeFormat;
    }

    IOParameterSet::FileMode IOParameterSet::parseFileMode(const std::string& modeString)
    {
        if(modeString == "unsigned8")
        {
            return UNSIGNED_8;
        }
        else if(modeString == "unsigned16")
        {
            return UNSIGNED_16;
        }
        else if(modeString == "float32")
        {
            return FLOAT_32;
        }
        else if(modeString == "signed16")
        {
            return SIGNED_16;
        }
        else
        {
            throw Exception("Unknown file mode ");
        }
    }

    float IOParameterSet::XAxisTilt() const
    {
        return xAxisTilt;
    }

    std::ostream& operator<<(std::ostream& oss, IOParameterSet::FileMode mode)
    {
        switch(mode)
        {
        case IOParameterSet::FLOAT_32:
            oss << "gray scale (32 bit float)";
            break;
        case IOParameterSet::UNSIGNED_8:
            oss << "gray scale (8 bit unsigned)";
            break;
        case IOParameterSet::UNSIGNED_16:
            oss << "gray scale (16 bit unsigned)";
            break;
        case IOParameterSet::SIGNED_16:
            oss << "gray scale (16 bit signed)";
            break;
        }
        return oss;
    }
}
