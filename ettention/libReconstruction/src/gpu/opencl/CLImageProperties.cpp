#include "stdafx.h"
#include "CLImageProperties.h"

namespace ettention
{
    CLImageProperties::CLImageProperties()
        : addressingMode(CL_ADDRESS_CLAMP_TO_EDGE)
        , channelOrder(CL_R)
        , channelType(CL_FLOAT)
        , filterMode(CL_FILTER_LINEAR)
        , normalizedCoordinates(true) 
        , resolutionType(TYPE_UNKNOWN)
        , resolution(0, 0, 0)
    {
    }

    CLImageProperties::~CLImageProperties()
    {
    }

    cl_addressing_mode CLImageProperties::getAddressingMode() const
    {
        return addressingMode;
    }

    void CLImageProperties::setAddressingMode(cl_addressing_mode addressingMode)
    {
        this->addressingMode = addressingMode;
    }

    cl_channel_order CLImageProperties::getChannelOrder() const
    {
        return channelOrder;
    }

    void CLImageProperties::setChannelOrder(cl_channel_order channelOrder)
    {
        this->channelOrder = channelOrder;
    }

    cl_channel_type CLImageProperties::getChannelType() const
    {
        return channelType;
    }

    void CLImageProperties::setChannelType(cl_channel_type channelType)
    {
        this->channelType = channelType;
    }

    cl_filter_mode CLImageProperties::getFilterMode() const
    {
        return filterMode;
    }

    void CLImageProperties::setFilterMode(cl_filter_mode filterMode)
    {
        this->filterMode = filterMode;
    }

    bool CLImageProperties::getNormalizedCoordinates() const
    {
        return normalizedCoordinates;
    }

    void CLImageProperties::setNormalizedCoordinates(bool normalizedCoordinates)
    {
        this->normalizedCoordinates = normalizedCoordinates;
    }

    void CLImageProperties::setResolution(unsigned int resolution)
    {
        throw Exception("1D images not supported!");
    }

    void CLImageProperties::setResolution(const Vec2ui& resolution)
    {
        this->resolution = Vec3ui(resolution, 1);
        resolutionType = TYPE_2D;
    }

    void CLImageProperties::setResolution(const Vec3ui& resolution)
    {
        this->resolution = resolution;
        resolutionType = TYPE_3D;
    }

    CLImageProperties::ResolutionType CLImageProperties::getResolutionType() const
    {
        return resolutionType;
    }

    const Vec3ui& CLImageProperties::getResolution() const
    {
        return resolution;
    }

    const std::string& CLImageProperties::getImageName() const
    {
        return imageName;
    }

    void CLImageProperties::setImageName(const std::string& imageName)
    {
        this->imageName = imageName;
    }

    std::size_t CLImageProperties::getByteWidthOfPixel() const
    {
        switch(channelType)
        {
        case CL_FLOAT:
            return sizeof(float);
        case CL_HALF_FLOAT:
            return sizeof(short);
        case CL_UNSIGNED_INT8:
            return sizeof(unsigned char);
        case CL_UNSIGNED_INT16:
            return sizeof(unsigned short);
        case CL_UNSIGNED_INT32:
            return sizeof(unsigned int);
        case CL_SIGNED_INT8:
            return sizeof(char);
        case CL_SIGNED_INT16:
            return sizeof(short);
        case CL_SIGNED_INT32:
            return sizeof(int);
        case CL_UNORM_INT8:
            return sizeof(char);
        case CL_UNORM_INT16:
            return sizeof(short);
        default:
            throw GPUException("OpenCL channel type not supported!");
        }
    }

    std::size_t CLImageProperties::getImageByteWidth() const
    {
        return (std::size_t)resolution.x * (std::size_t)resolution.y * (std::size_t)resolution.z * getByteWidthOfPixel();
    }
}