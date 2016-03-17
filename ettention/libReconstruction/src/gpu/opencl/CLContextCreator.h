#pragma once
#include <vector>
#include <string>
#include "gpu/opencl/OpenCLHeader.h"
#include "setup/parameterset/HardwareParameterSet.h"

namespace ettention
{
    class CLContextCreator
    {
    public:
        struct DeviceInfo
        {
            std::string name;
            std::string type;
            std::string version;
            std::string platform;
            std::vector<std::string> extensions;
        };

        CLContextCreator();
        ~CLContextCreator();

        cl_context CreateContext(const HardwareParameterSet* hardwareParameterSet);
        std::vector<DeviceInfo> GatherDeviceInfos() const;

    private:
        typedef std::pair<cl_platform_id, cl_device_id> PlatformDevice;

        cl_context TryCreateContextFromDevices(const std::vector<PlatformDevice>& platformDevices) const;
        cl_context CreateContextFromDevice(const PlatformDevice& platformDevice) const;
        std::vector<PlatformDevice> GetPreferredDevices(HardwareParameterSet::OpenCLPlatformVendor vendor, HardwareParameterSet::OpenCLDevice device) const;
        void FillAvailableDevices();
        void FillAvailableDevicesFromPlatform(cl_platform_id platform);
        DeviceInfo GetDeviceInfo(const PlatformDevice& platformDevice) const;        
        std::string GetSinglePlatformInfo(cl_platform_id platform, cl_platform_info info) const;

        template <typename _InfoType>
        _InfoType GetSingleDeviceInfo(cl_device_id device, cl_device_info info) const;

        std::vector<PlatformDevice> availableDevices;
    };

    template <typename _InfoType>
    _InfoType CLContextCreator::GetSingleDeviceInfo(cl_device_id device, cl_device_info info) const
    {
        size_t size = 0;
        CL_ASSERT(clGetDeviceInfo(device, info, 0, 0, &size));
        if(size != sizeof(_InfoType))
        {
            throw GPUException("Template specialization type size does not fit to return value of clGetDeviceInfo!");
        }
        _InfoType result;
        CL_ASSERT(clGetDeviceInfo(device, info, size, &result, 0));
        return result;
    }

    template <>
    std::string CLContextCreator::GetSingleDeviceInfo(cl_device_id device, cl_device_info info) const;
}