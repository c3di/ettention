#pragma once
#include "setup/parameterset/ParameterSet.h"

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
        OpenCLPlatformVendor parseOpenCLPlatform(const std::string value) const;
        OpenCLDevice parseOpenCLDevice(const std::string value) const;

        OpenCLPlatformVendor preferedOpenCLPlatform;
        OpenCLDevice openCLDevice;
        int explicitDeviceId;
        bool printDeviceInfo;
        bool disableImageSupport;
        unsigned int subVolumeCount;

    public:
        static const unsigned int NO_EXPLICIT_DEVICE_ID = (unsigned int)-1;
        static const unsigned int SUBVOLUME_COUNT_AUTO = 0U;

        HardwareParameterSet(const ParameterSource* parameterSource);
        HardwareParameterSet(OpenCLPlatformVendor platformVendor, OpenCLDevice device);
        ~HardwareParameterSet();

        OpenCLPlatformVendor getPreferedOpenCLPlatform() const;
        OpenCLDevice getPreferedOpenCLDevice() const;
        const unsigned int& getSubVolumeCount() const;
        int getExplicitDeviceId() const;
        bool shouldPrintDeviceInfo() const;
        bool isImageSupportDisabled() const;
    };
}
