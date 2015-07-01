#include "stdafx.h"

#include "OutputFormatParameterSet.h"

#include "framework/error/Exception.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    OutputFormatParameterSet::OutputFormatParameterSet(VoxelValueType voxelType, CoordinateOrder coordinateOrder, bool invertData)
        : voxelType(voxelType), coordinateOrder(coordinateOrder), invertData(invertData)
    {
    }

    OutputFormatParameterSet::OutputFormatParameterSet(const ParameterSource* parameterSource)
    {
        voxelType = parseVoxelType(parameterSource->getStringParameter("output.options.voxelType"));

        invertData = false;
        if(parameterSource->parameterExists("output.options.invert"))
            invertData = parameterSource->getBoolParameter("output.options.invert");

        coordinateOrder = ORDER_XZY;
        if(parameterSource->parameterExists("output.options.orientation"))
        {
            coordinateOrder = parseVolumeRotation(parameterSource->getStringParameter("output.options.orientation"));
        }
    }

    OutputFormatParameterSet::~OutputFormatParameterSet()
    {
    }

    OutputFormatParameterSet::VoxelValueType OutputFormatParameterSet::VoxelType() const
    {
        return voxelType;
    }

    bool OutputFormatParameterSet::InvertData() const
    {
        return invertData;
    }

    OutputFormatParameterSet::CoordinateOrder OutputFormatParameterSet::OutputVolumeRotation() const
    {
        return coordinateOrder;
    }

    OutputFormatParameterSet::CoordinateOrder OutputFormatParameterSet::parseVolumeRotation(const std::string rotationString)
    {
        if(rotationString == "xzy_order")
        {
            return ORDER_XZY;
        }
        else if(rotationString == "xyz_order")
        {
            return ORDER_XYZ;
        }
        else
        {
            throw Exception("Unknown output volume rotation constant");
        }
    }

    OutputFormatParameterSet::VoxelValueType OutputFormatParameterSet::parseVoxelType(const std::string modeString)
    {
        if(modeString == "unsigned8")
        {
            return OutputFormatParameterSet::UNSIGNED_8;
        }
        else if(modeString == "unsigned16")
        {
            return OutputFormatParameterSet::UNSIGNED_16;
        }
        else if(modeString == "float32")
        {
            return OutputFormatParameterSet::FLOAT_32;
        }
        else if(modeString == "signed16")
        {
            return OutputFormatParameterSet::SIGNED_16;
        }
        else
        {
            throw Exception("Unknown file mode ");
        }
    }

    std::ostream& operator<<(std::ostream& oss, OutputFormatParameterSet::VoxelValueType mode)
    {
        switch(mode)
        {
        case OutputFormatParameterSet::FLOAT_32:
            oss << "gray scale (32 bit float)";
            break;
        case OutputFormatParameterSet::UNSIGNED_8:
            oss << "gray scale (8 bit unsigned)";
            break;
        case OutputFormatParameterSet::UNSIGNED_16:
            oss << "gray scale (16 bit unsigned)";
            break;
        case OutputFormatParameterSet::SIGNED_16:
            oss << "gray scale (16 bit signed)";
            break;
        }
        return oss;
    }
}