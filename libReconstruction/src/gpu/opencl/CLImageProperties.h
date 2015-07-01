#pragma once
#include "gpu/opencl/OpenCLHeader.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class CLImageProperties
    {
    public:
        enum ResolutionType
        {
            TYPE_UNKNOWN,
            TYPE_2D,
            TYPE_3D,
        };

        CLImageProperties();
        ~CLImageProperties();

        cl_addressing_mode getAddressingMode() const;
        void setAddressingMode(cl_addressing_mode addressingMode);
        cl_channel_order getChannelOrder() const;
        void setChannelOrder(cl_channel_order channelOrder);
        cl_channel_type getChannelType() const;
        void setChannelType(cl_channel_type channelType);
        cl_filter_mode getFilterMode() const;
        void setFilterMode(cl_filter_mode filterMode);
        bool getNormalizedCoordinates() const;
        void setNormalizedCoordinates(bool normalizedCoordinates);
        void setResolution(unsigned int resolution);
        void setResolution(const Vec2ui& resolution);
        void setResolution(const Vec3ui& resolution);
        ResolutionType getResolutionType() const;
        const Vec3ui& getResolution() const;
        const std::string& getImageName() const;
        void setImageName(const std::string& imageName);
        std::size_t getImageByteWidth() const;
        std::size_t getByteWidthOfPixel() const;

    private:
        cl_addressing_mode addressingMode;
        cl_channel_order channelOrder;
        cl_channel_type channelType;
        cl_filter_mode filterMode;
        bool normalizedCoordinates;
        ResolutionType resolutionType;
        Vec3ui resolution;
        std::string imageName;
    };
}