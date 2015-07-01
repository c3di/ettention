#pragma once
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class HardwareParameterSet : public ParameterSet
    {
    public:
        enum OpenCLPlatformVendor
        {
            any,
            nvidia,
            intel,
            amd,
        };

        enum OpenCLDevice
        {
            GPU_first,
            CPU_first,
            GPU_only,
            CPU_only,
        };

    private:
        OpenCLPlatformVendor ParseOpenCLPlatform(const std::string value) const;
        OpenCLDevice ParseOpenCLDevice(const std::string value) const;

        OpenCLPlatformVendor preferedOpenCLPlatform;
        OpenCLDevice openCLDevice;
        int explicitDeviceId;
        bool printDeviceInfo;
        bool disableImageSupport;
        boost::optional<unsigned int> subVolumeCount;

    public:
        static const unsigned int NO_EXPLICIT_DEVICE_ID = (unsigned int)-1;

        HardwareParameterSet(const ParameterSource* parameterSource);
        HardwareParameterSet(OpenCLPlatformVendor platformVendor, OpenCLDevice device);
        ~HardwareParameterSet();

        OpenCLPlatformVendor PreferedOpenCLPlatform() const;
        OpenCLDevice PreferedOpenCLDevice() const;
        const boost::optional<unsigned int>& SubVolumeCount() const;;
        int ExplicitDeviceId() const;
        bool PrintDeviceInfo() const;
        bool DisableImageSupport() const;
    };
}
