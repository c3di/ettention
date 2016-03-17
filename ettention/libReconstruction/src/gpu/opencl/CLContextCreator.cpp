#include "stdafx.h"
#include "CLContextCreator.h"
#include "framework/Logger.h"

namespace ettention
{
    CLContextCreator::CLContextCreator()
    {
        this->FillAvailableDevices();
    }

    CLContextCreator::~CLContextCreator()
    {

    }

    cl_context CLContextCreator::CreateContext(const HardwareParameterSet* hardwareParameterSet)
    {
        if(availableDevices.empty())
        {
            throw GPUException("No OpenCL devices available on this machine!");
        }
        cl_context context = 0;
        if(hardwareParameterSet->getExplicitDeviceId() != -1)
        {
            if(hardwareParameterSet->getExplicitDeviceId() < (int)availableDevices.size())
            {
                auto platformDevice = availableDevices[hardwareParameterSet->getExplicitDeviceId()];
                context = this->CreateContextFromDevice(platformDevice);
            }
            else
            {
                LOGGER_IMP("Explicit device id " << hardwareParameterSet->getExplicitDeviceId() << " is invalid.");
            }
        }
        if(!context)
        {
            auto devices = this->GetPreferredDevices(hardwareParameterSet->getPreferedOpenCLPlatform(), hardwareParameterSet->getPreferedOpenCLDevice());
            context = this->TryCreateContextFromDevices(devices);
        }
        if(!context)
        {
            context = this->TryCreateContextFromDevices(availableDevices);
        }
        if(!context)
        {
            throw GPUException("CL context creation failed for every single device this machine offers!");
        }
        return context;
    }

    cl_context CLContextCreator::TryCreateContextFromDevices(const std::vector<PlatformDevice>& platformDevices) const
    {
        for(auto it = platformDevices.begin(); it != platformDevices.end(); ++it)
        {
            auto context = this->CreateContextFromDevice(*it);
            if(context)
            {
                return context;
            }
        }
        return 0;
    }

    cl_context CLContextCreator::CreateContextFromDevice(const PlatformDevice& platformDevice) const
    {
        cl_context_properties props[3] =
        {
            CL_CONTEXT_PLATFORM, (cl_context_properties)platformDevice.first,
            0,
        };
        cl_int err;
        auto context = clCreateContext(props, 1, &platformDevice.second, 0, 0, &err);
        CL_ASSERT(err);
        auto info = this->GetDeviceInfo(platformDevice);
        LOGGER_IMP("OpenCL device: " << info.name << ".");
        return context;
    }

    std::vector<CLContextCreator::PlatformDevice> CLContextCreator::GetPreferredDevices(HardwareParameterSet::OpenCLPlatformVendor vendor, HardwareParameterSet::OpenCLDevice device) const
    {
        std::map<HardwareParameterSet::OpenCLPlatformVendor, std::string> knownVendorNames;
        knownVendorNames[HardwareParameterSet::amd] = "Advanced Micro Devices, Inc.";
        knownVendorNames[HardwareParameterSet::intel] = "Intel(R) Corporation";
        knownVendorNames[HardwareParameterSet::nvidia] = "NVIDIA Corporation";
        std::vector<PlatformDevice> candidates;
        if(vendor != HardwareParameterSet::any)
        {
            for(auto it = availableDevices.begin(); it != availableDevices.end(); ++it)
            {
                if(this->GetSinglePlatformInfo(it->first, CL_PLATFORM_VENDOR) == knownVendorNames[vendor])
                {
                    candidates.push_back(*it);
                }
            }
            if(candidates.empty())
            {
                LOGGER_IMP("No devices for preferred platform found, using any platform.");
            }
        }
        if(candidates.empty())
        {
            candidates = availableDevices;
        }
        std::vector<PlatformDevice> allowedDevices;
        bool isCpuAllowed = device == HardwareParameterSet::CPU_first || device == HardwareParameterSet::CPU_only;
        bool isGpuAllowed = device == HardwareParameterSet::GPU_first || device == HardwareParameterSet::GPU_only;
        bool isAccAllowed = device == HardwareParameterSet::CPU_first || device == HardwareParameterSet::GPU_first;
        for(auto it = candidates.begin(); it != candidates.end(); ++it)
        {
            bool isCpu = this->GetSingleDeviceInfo<cl_device_type>(it->second, CL_DEVICE_TYPE) == CL_DEVICE_TYPE_CPU;
            bool isGpu = this->GetSingleDeviceInfo<cl_device_type>(it->second, CL_DEVICE_TYPE) == CL_DEVICE_TYPE_GPU;
            bool isAcc = this->GetSingleDeviceInfo<cl_device_type>(it->second, CL_DEVICE_TYPE) == CL_DEVICE_TYPE_ACCELERATOR;
            if((isCpu && isCpuAllowed) || (isGpu && isGpuAllowed) || (isAcc && isAccAllowed))
            {
                bool hasHighPriority = (isCpu && device == HardwareParameterSet::CPU_first) || (isGpu && device == HardwareParameterSet::GPU_first);
                if(hasHighPriority)
                {
                    allowedDevices.insert(allowedDevices.begin(), *it);
                }
                else
                {
                    allowedDevices.push_back(*it);
                }
            }
        }
        return allowedDevices;
    }

    void CLContextCreator::FillAvailableDevices()
    {
        cl_uint count = 0;
        CL_ASSERT(clGetPlatformIDs(0, 0, &count));
        std::vector<cl_platform_id> platforms(count);
        CL_ASSERT(clGetPlatformIDs(count, &platforms[0], 0));
        for(auto it = platforms.begin(); it != platforms.end(); ++it)
        {
            this->FillAvailableDevicesFromPlatform(*it);
        }
    }

    void CLContextCreator::FillAvailableDevicesFromPlatform(cl_platform_id platform)
    {
        cl_uint count = 0;
        std::vector<cl_device_id> devices;
        CL_ASSERT(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, 0, &count));
        devices.resize(count);
        CL_ASSERT(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, count, &devices[0], 0));
        for(auto it = devices.begin(); it != devices.end(); ++it)
        {
            availableDevices.push_back(std::make_pair(platform, *it));
        }
    }

    std::vector<CLContextCreator::DeviceInfo> CLContextCreator::GatherDeviceInfos() const
    {
        std::vector<CLContextCreator::DeviceInfo> infos;
        for(auto it = availableDevices.begin(); it != availableDevices.end(); ++it)
        {
            infos.push_back(this->GetDeviceInfo(*it));
        }
        return infos;
    }

    CLContextCreator::DeviceInfo CLContextCreator::GetDeviceInfo(const PlatformDevice& platformDevice) const
    {
        DeviceInfo info =
        {
            this->GetSingleDeviceInfo<std::string>(platformDevice.second, CL_DEVICE_NAME),
            "???",
            this->GetSingleDeviceInfo<std::string>(platformDevice.second, CL_DEVICE_VERSION),
            this->GetSinglePlatformInfo(platformDevice.first, CL_PLATFORM_NAME),
            std::vector<std::string>(),
        };
        switch(this->GetSingleDeviceInfo<cl_device_type>(platformDevice.second, CL_DEVICE_TYPE))
        {
        case CL_DEVICE_TYPE_ACCELERATOR:
            info.type = "ACC";
            break;
        case CL_DEVICE_TYPE_CPU:
            info.type = "CPU";
            break;
        case CL_DEVICE_TYPE_GPU:
            info.type = "GPU";
            break;
        }
        std::stringstream extensions;
        extensions << this->GetSingleDeviceInfo<std::string>(platformDevice.second, CL_DEVICE_EXTENSIONS);
        while(extensions.good())
        {
            std::string extension;
            extensions >> extension;
            info.extensions.push_back(extension);
        }
        return info;
    }

    template <>
    std::string CLContextCreator::GetSingleDeviceInfo(cl_device_id device, cl_device_info info) const
    {
        size_t size = 0;
        CL_ASSERT(clGetDeviceInfo(device, info, 0, 0, &size));
        char* buffer = new char[size];
        CL_ASSERT(clGetDeviceInfo(device, info, size, buffer, 0));
        std::string result(buffer);
        delete[] buffer;
        return result;
    }

    std::string CLContextCreator::GetSinglePlatformInfo(cl_platform_id platform, cl_platform_info info) const
    {
        size_t size = 0;
        CL_ASSERT(clGetPlatformInfo(platform, info, 0, 0, &size));
        char* buffer = new char[size];
        CL_ASSERT(clGetPlatformInfo(platform, info, size, buffer, 0));
        std::string val(buffer);
        delete buffer;
        return val;
    }
}