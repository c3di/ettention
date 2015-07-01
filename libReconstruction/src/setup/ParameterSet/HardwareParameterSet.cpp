#include "stdafx.h"
#include "HardwareParameterSet.h"
#include "framework/error/Exception.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    HardwareParameterSet::HardwareParameterSet(const ParameterSource* parameterSource)
    {
        preferedOpenCLPlatform = OpenCLPlatformVendor::nvidia;
        if(parameterSource->parameterExists("hardware.openclVendor"))
        {
            preferedOpenCLPlatform = ParseOpenCLPlatform(parameterSource->getStringParameter("hardware.openclVendor"));
        }

        openCLDevice = OpenCLDevice::GPU_first;
        if(parameterSource->parameterExists("hardware.openclDeviceType"))
        {
            openCLDevice = ParseOpenCLDevice(parameterSource->getStringParameter("hardware.openclDeviceType"));
        }

        if(parameterSource->parameterExists("hardware.subVolumeCount"))
        {
            subVolumeCount = parameterSource->getUIntParameter("hardware.subVolumeCount");
        }

        printDeviceInfo = false;
        if(parameterSource->parameterExists("printDeviceInfo"))
        {
            printDeviceInfo = parameterSource->getBoolParameter("printDeviceInfo");
        }

        explicitDeviceId = NO_EXPLICIT_DEVICE_ID;
        if(parameterSource->parameterExists("hardware.openclDeviceId"))
        {
            if(parameterSource->parameterExists("hardware.openclDeviceType"))
                throw Exception("parameters hardware.openclDeviceId and hardware.openclDeviceType are exclusive");
            if(parameterSource->parameterExists("hardware.openclVendor"))
                throw Exception("parameters hardware.openclDeviceId and hardware.openclVendor are exclusive");
            explicitDeviceId = parameterSource->getIntParameter("hardware.openclDeviceId ");
        }

        disableImageSupport = false;
        if(parameterSource->parameterExists("hardware.disableImageSupport"))
        {
            disableImageSupport = parameterSource->getBoolParameter("hardware.disableImageSupport");
        }
    }

    HardwareParameterSet::HardwareParameterSet(OpenCLPlatformVendor platformVendor, OpenCLDevice device)
        : preferedOpenCLPlatform(platformVendor)
        , openCLDevice(device)
        , printDeviceInfo(false)
        , explicitDeviceId(NO_EXPLICIT_DEVICE_ID)
        , disableImageSupport(false)
    {
    }

    HardwareParameterSet::~HardwareParameterSet()
    {
    }

    HardwareParameterSet::OpenCLPlatformVendor HardwareParameterSet::PreferedOpenCLPlatform() const
    {
        return preferedOpenCLPlatform;
    }

    HardwareParameterSet::OpenCLDevice HardwareParameterSet::PreferedOpenCLDevice() const
    {
        return openCLDevice;
    }

    int HardwareParameterSet::ExplicitDeviceId() const
    {
        return explicitDeviceId;
    }

    HardwareParameterSet::OpenCLPlatformVendor HardwareParameterSet::ParseOpenCLPlatform(const std::string value) const
    {
        if(value == "any")
        {
            return OpenCLPlatformVendor::any;
        }
        if(value == "nvidia")
        {
            return OpenCLPlatformVendor::nvidia;
        }
        if(value == "intel")
        {
            return OpenCLPlatformVendor::intel;
        }
        if(value == "amd")
        {
            return OpenCLPlatformVendor::amd;
        }
        throw std::runtime_error((boost::format("Illegal parameter value for OpenCLPlatformVendor : %1%") % value).str().c_str());
    }

    HardwareParameterSet::OpenCLDevice HardwareParameterSet::ParseOpenCLDevice(const std::string value) const
    {
        if(value == "gpu_cpu")
        {
            return OpenCLDevice::GPU_first;
        }
        if(value == "cpu_gpu")
        {
            return OpenCLDevice::CPU_first;
        }
        if(value == "gpu")
        {
            return OpenCLDevice::GPU_only;
        }
        if(value == "cpu")
        {
            return OpenCLDevice::CPU_only;
        }
        throw std::runtime_error((boost::format("Illegal parameter value for OpenCL_Device : %1%") % value).str().c_str());
    }

    bool HardwareParameterSet::PrintDeviceInfo() const
    {
        return printDeviceInfo;
    }

    bool HardwareParameterSet::DisableImageSupport() const
    {
        return disableImageSupport;
    }

    const boost::optional<unsigned int>& HardwareParameterSet::SubVolumeCount() const
    {
        return subVolumeCount;
    }
}