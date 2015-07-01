#include "stdafx.h"
#include "CLAbstractionLayer.h"
#include <CL/opencl.h>
#include "CLImageProperties.h"
#include "CLMemObject.h"
#include "CLMemoryStructure.h"
#include "CLSampler.h"
#include "framework/Framework.h"
#include "framework/Logger.h"
#include "framework/time/PerformanceReport.h"
#include "setup/ParameterSet/DebugParameterSet.h"
#include "gpu/SingleInstanceTimestamps.h"
#include "model/volume/VoxelType.h"
#include "setup/ParameterSet/AlgebraicParameterSet.h"

namespace ettention
{
    template <typename T>
    T  CLAbstractionLayer::getDeviceInfo(cl_device_id devID, cl_device_info devInfo)
    {
        T retInfo;
        CL_ASSERT(clGetDeviceInfo(devID, devInfo, sizeof(retInfo), &retInfo, NULL));
        return retInfo;
    }

    template<>
    std::string CLAbstractionLayer::getDeviceInfo<std::string>(cl_device_id deviceId, cl_device_info info)
    {
        size_t charCount = 0;
        CL_ASSERT(clGetDeviceInfo(deviceId, info, 0, 0, &charCount));
        std::string result;
        result.resize(charCount);
        CL_ASSERT(clGetDeviceInfo(deviceId, info, charCount, &result[0], 0));
        return result;
    }

    CLAbstractionLayer::CLAbstractionLayer(Framework* framework)
        : framework(framework)
        , context(0)
        , devices(0)
        , imageSupport(false)
    {
        const HardwareParameterSet* hardwareParameterSet = framework->getParameterSet()->get<HardwareParameterSet>();
        context = contextCreator.CreateContext(hardwareParameterSet);
        listDevices();
        createCommandQueue();
        getMemoryInformationOnDevice(getDeviceId());
        checkImageSupport(hardwareParameterSet->DisableImageSupport());
        if(hardwareParameterSet->PrintDeviceInfo())
        {
            printSingleDeviceInfo(getDeviceId());
        }
    }

    CLAbstractionLayer::~CLAbstractionLayer()
    {
        if(!incompleteEvents.empty())
        {
            CL_ASSERT(clWaitForEvents((cl_uint)incompleteEvents.size(), &incompleteEvents[0]));
            profileCachedEvents();
        }
        CL_ASSERT(clReleaseCommandQueue(queue));
        CL_ASSERT(clReleaseContext(context));
        if(aliveCLMemObjects.size() + aliveCLSamplers.size() != 0)
        {
            std::stringstream message;
            message << "Still " << aliveCLMemObjects.size() << " unreturned CLMemObject(s) ";
            message << "and " << aliveCLSamplers.size() << " CLSampler(s)!";
            throw GPUException(message.str());
        }
    }

    std::size_t CLAbstractionLayer::getRemainingMemorySizeInBytes()
    {
        return (size_t)remainingMemoryInBytes;
    }

    size_t CLAbstractionLayer::getMaxSizeOfIndividualMemoryObjectInBytes()
    {
        cl_ulong maxSizeOfIndividualMemoryObject;
        CL_ASSERT(clGetDeviceInfo(devices[0], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &maxSizeOfIndividualMemoryObject, NULL));
        return (size_t)maxSizeOfIndividualMemoryObject;
    }

    void CLAbstractionLayer::createCommandQueue()
    {
        cl_command_queue_properties queueProperties = 0;

        isProfilingEnabled = framework->getParameterSet()->get<DebugParameterSet>()->GetProfilingLevel() >= DebugParameterSet::PROFILING_NORMAL;
        if(isProfilingEnabled)
           queueProperties |= CL_QUEUE_PROFILING_ENABLE;

        cl_int err;
        queue = clCreateCommandQueue(context, devices[0], queueProperties, &err);
        CL_ASSERT(err);
    }

    cl_device_id CLAbstractionLayer::getDeviceId()
    {
        return devices[0];
    }

    cl_context CLAbstractionLayer::getContext()
    {
        return context;
    }

    cl_command_queue CLAbstractionLayer::getCommandQueue()
    {
        return queue;
    }

    void CLAbstractionLayer::allocate(std::size_t byteWidth)
    {
        if(byteWidth > remainingMemoryInBytes)
        {
            std::stringstream str;
            str << "Allocation of " << byteWidth << "B failed because there is only " << remainingMemoryInBytes << "B left.";
            throw GPUException(str.str());
        }
        remainingMemoryInBytes -= byteWidth;
    }

    void CLAbstractionLayer::deallocate(std::size_t byteWidth)
    {
        remainingMemoryInBytes += byteWidth;
    }

    void CLAbstractionLayer::registerCLSampler(cl_sampler clSampler)
    {
        aliveCLSamplers.insert(clSampler);
    }

    void CLAbstractionLayer::unregisterCLSampler(cl_sampler clSampler)
    {
        aliveCLSamplers.erase(clSampler);
    }

    void CLAbstractionLayer::registerCLMem(cl_mem clMem)
    {
        aliveCLMemObjects.insert(clMem);
    }

    void CLAbstractionLayer::unregisterCLMem(cl_mem clMem)
    {
        aliveCLMemObjects.erase(clMem);
    }

    void CLAbstractionLayer::listDevices()
    {
        size_t sizeOfDeviceArray;
        CL_ASSERT(clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &sizeOfDeviceArray));

        size_t deviceCount = sizeOfDeviceArray / sizeof(cl_device_id);
        devices.resize(deviceCount);

        CL_ASSERT(clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeOfDeviceArray, &devices[0], NULL));
    }

    void CLAbstractionLayer::getMemoryInformationOnDevice(cl_device_id deviceId)
    {
        cl_ulong globalMemSize;
        CL_ASSERT(clGetDeviceInfo(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &globalMemSize, NULL));
        remainingMemoryInBytes = globalMemSize;
    }

    void CLAbstractionLayer::checkImageSupport(bool forceDisable)
    {
        cl_bool imageSupport = getDeviceInfo<cl_bool>(devices[0], CL_DEVICE_IMAGE_SUPPORT);
        if(imageSupport && forceDisable)
        {
            this->imageSupport = false;
            LOGGER("The OpenCL device provides image support but it's deactivated by manual configuration.");
        }
        else
        {
            this->imageSupport = imageSupport == CL_TRUE;
        }
    }

    void CLAbstractionLayer::printSingleDeviceInfo(cl_device_id deviceId)
    {
        LOGGER_INFO_FORMATTED("\t Device Type:");
        LOGGER_INFO_FORMATTED("\t\t CL_DEVICE_TYPE_ACCELERATOR" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_TYPE) & CL_DEVICE_TYPE_ACCELERATOR ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t CL_DEVICE_TYPE_CPU" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_TYPE) & CL_DEVICE_TYPE_CPU ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t CL_DEVICE_TYPE_DEFAULT" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_TYPE) & CL_DEVICE_TYPE_DEFAULT ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t CL_DEVICE_TYPE_GPU" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_TYPE) & CL_DEVICE_TYPE_GPU ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t Device ID" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_VENDOR_ID));
        LOGGER_INFO_FORMATTED("\t\t  Max compute units" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS));
        LOGGER_INFO_FORMATTED("\t\t Max work items dimensions" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS));
        size_t workItemsSize;
        CL_ASSERT(clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, NULL, &workItemsSize));
        size_t *witems = new size_t[workItemsSize / sizeof(size_t)];
        CL_ASSERT(clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, workItemsSize, witems, NULL));
        cl_uint nWorkItems = getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
        for(unsigned int x = 0; x < nWorkItems; x++)
        {
            LOGGER_INFO_FORMATTED("\t Max work items[" << x << "] " << witems[x]);
        }
        delete[] witems;
        LOGGER_INFO_FORMATTED("\t Max work group size" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE));
        LOGGER_INFO_FORMATTED("\t Preferred vector width char" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR));
        LOGGER_INFO_FORMATTED("\t Preferred vector width short" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT));
        LOGGER_INFO_FORMATTED("\t Preferred vector width int" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT));
        LOGGER_INFO_FORMATTED("\t Preferred vector width long" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG));
        LOGGER_INFO_FORMATTED("\t Preferred vector width float" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT));
        LOGGER_INFO_FORMATTED("\t Preferred vector width double" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE));
        LOGGER_INFO_FORMATTED("\t Preferred vector width half" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF));
        LOGGER_INFO_FORMATTED("\t Max clock frequency" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_CLOCK_FREQUENCY) << "Mhz");
        LOGGER_INFO_FORMATTED("\t Address bits" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_ADDRESS_BITS));
        LOGGER_INFO_FORMATTED("\t Max memeory allocation" << getDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_MAX_MEM_ALLOC_SIZE));
        LOGGER_INFO_FORMATTED("\t Image support" << (getDeviceInfo<cl_bool>(deviceId, CL_DEVICE_IMAGE_SUPPORT) ? "Yes" : "No"));
        if(getDeviceInfo<cl_bool>(deviceId, CL_DEVICE_IMAGE_SUPPORT))
        {
            LOGGER_INFO_FORMATTED("\t\t Max number of images read arguments" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_READ_IMAGE_ARGS));
            LOGGER_INFO_FORMATTED("\t\t Max number of images write arguments" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_WRITE_IMAGE_ARGS));
            LOGGER_INFO_FORMATTED("\t\t Max image 2D width" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_IMAGE2D_MAX_WIDTH));
            LOGGER_INFO_FORMATTED("\t\t Max image 2D height" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_IMAGE2D_MAX_HEIGHT));
            LOGGER_INFO_FORMATTED("\t\t Max image 3D width" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_IMAGE3D_MAX_WIDTH));
            LOGGER_INFO_FORMATTED("\t\t Max image 3D height" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_IMAGE3D_MAX_HEIGHT));
            LOGGER_INFO_FORMATTED("\t\t Max image 3D depth" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_IMAGE3D_MAX_DEPTH));
            LOGGER_INFO_FORMATTED("\t\t Max samplers within kernel" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_SAMPLERS));
        }
        LOGGER_INFO_FORMATTED("\t Max size of kernel argument" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_MAX_PARAMETER_SIZE));
        LOGGER_INFO_FORMATTED("\t Alignment (bits) of base address" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MEM_BASE_ADDR_ALIGN));
        LOGGER_INFO_FORMATTED("\t Minimum alignment (bytes) for any datatype" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE));
        LOGGER_INFO_FORMATTED("\t Single precision floating point capabilities");
        LOGGER_INFO_FORMATTED("\t\t Denorms" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG) & CL_FP_DENORM ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t INF and quiet NaN" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG) & CL_FP_INF_NAN ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t Round to nearest even" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG) & CL_FP_ROUND_TO_NEAREST ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t Round to zero" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG) & CL_FP_ROUND_TO_ZERO ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t Round to positive and negative INF" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG) & CL_FP_ROUND_TO_INF ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t IEEE754-2008 fused multiply-add" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG) & CL_FP_FMA ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t Basic operations implemented in SW:" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_SINGLE_FP_CONFIG) & CL_FP_SOFT_FLOAT ? "Yes" : "No"));
        switch(getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE))
        {
        case CL_NONE:
            LOGGER_INFO_FORMATTED("None");
            break;
        case CL_READ_ONLY_CACHE:
            LOGGER_INFO_FORMATTED("Read only");
            break;
        case CL_READ_WRITE_CACHE:
            LOGGER_INFO_FORMATTED("Read/Write");
            break;
        }
        LOGGER_INFO_FORMATTED("\t Cache line size" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE));
        LOGGER_INFO_FORMATTED("\t Cache size" << getDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE));
        LOGGER_INFO_FORMATTED("\t Global memory size" << getDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_GLOBAL_MEM_SIZE));
        LOGGER_INFO_FORMATTED("\t Constant buffer size" << getDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE));
        LOGGER_INFO_FORMATTED("\t Max number of constant args" << getDeviceInfo<cl_uint>(deviceId, CL_DEVICE_MAX_CONSTANT_ARGS));
        LOGGER_INFO_FORMATTED("  Local memory type");
        switch(getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_LOCAL_MEM_TYPE))
        {
        case CL_LOCAL:
            LOGGER_INFO_FORMATTED("Local memory storage");
            break;
        case CL_GLOBAL:
            LOGGER_INFO_FORMATTED("Global");
            break;
        }
        LOGGER_INFO_FORMATTED("\t Local memory size" << getDeviceInfo<cl_ulong>(deviceId, CL_DEVICE_LOCAL_MEM_SIZE));
        LOGGER_INFO_FORMATTED("\t Error correction support" << (getDeviceInfo<cl_bool>(deviceId, CL_DEVICE_ERROR_CORRECTION_SUPPORT) ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t Profiling timer resolution" << getDeviceInfo<size_t>(deviceId, CL_DEVICE_PROFILING_TIMER_RESOLUTION));
        LOGGER_INFO_FORMATTED("\t Device endianess" << (getDeviceInfo<cl_bool>(deviceId, CL_DEVICE_ENDIAN_LITTLE) ? "Little" : "Big"));
        LOGGER_INFO_FORMATTED("\t Available" << (getDeviceInfo<cl_bool>(deviceId, CL_DEVICE_AVAILABLE) ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t Compiler available" << (getDeviceInfo<cl_bool>(deviceId, CL_DEVICE_COMPILER_AVAILABLE) ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t Execution capabilities");
        LOGGER_INFO_FORMATTED("\t\t Execute OpenCL kernels" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_EXECUTION_CAPABILITIES) & CL_EXEC_KERNEL ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t Execute native function" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_EXECUTION_CAPABILITIES) & CL_EXEC_NATIVE_KERNEL ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t Queue properties");
        LOGGER_INFO_FORMATTED("\t\t Out-of-Order" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_QUEUE_PROPERTIES) & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t\t Profiling" << (getDeviceInfo<cl_bitfield>(deviceId, CL_DEVICE_QUEUE_PROPERTIES) & CL_QUEUE_PROFILING_ENABLE ? "Yes" : "No"));
        LOGGER_INFO_FORMATTED("\t Platform ID" << getDeviceInfo<cl_platform_id>(deviceId, CL_DEVICE_PLATFORM));
        LOGGER_INFO_FORMATTED("\t Device name: " << getDeviceInfo<std::string>(deviceId, CL_DEVICE_NAME));
        LOGGER_INFO_FORMATTED("\t Vendor: " << getDeviceInfo<std::string>(deviceId, CL_DEVICE_VENDOR));
        LOGGER_INFO_FORMATTED("\t Driver Version: " << getDeviceInfo<std::string>(deviceId, CL_DRIVER_VERSION));
        LOGGER_INFO_FORMATTED("\t Profile: " << getDeviceInfo<std::string>(deviceId, CL_DEVICE_PROFILE));
        LOGGER_INFO_FORMATTED("\t Device Version: " << getDeviceInfo<std::string>(deviceId, CL_DEVICE_VERSION));
        LOGGER_INFO_FORMATTED("  OpenCL C language version: " << getDeviceInfo<std::string>(deviceId, CL_DEVICE_OPENCL_C_VERSION));
        LOGGER_INFO_FORMATTED("\t Extensions: " << getDeviceInfo<std::string>(deviceId, CL_DEVICE_EXTENSIONS));
    }

    void CLAbstractionLayer::profileAndReleaseEvent(const std::string& eventCaption, cl_event evt)
    {
        if(!isProfilingEnabled)
        {
            CL_ASSERT(clReleaseEvent(evt));
        }
        else
        {
            incompleteEvents.push_back(evt);
            incompleteEventCaptions.push_back(eventCaption);
            profileCachedEvents();
        }     
    }

    void CLAbstractionLayer::profileCachedEvents()
    {
        while(!incompleteEvents.empty())
        {
            cl_int status = 0;
            CL_ASSERT(clGetEventInfo(incompleteEvents.front(), CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(cl_int), &status, 0));
            if(status != CL_COMPLETE)
                break;

            unsigned long long queued, submitted, started, finished;
            CL_ASSERT(clGetEventProfilingInfo(incompleteEvents.front(), CL_PROFILING_COMMAND_QUEUED, sizeof(unsigned long long), &queued, 0));
            CL_ASSERT(clGetEventProfilingInfo(incompleteEvents.front(), CL_PROFILING_COMMAND_SUBMIT, sizeof(unsigned long long), &submitted, 0));
            CL_ASSERT(clGetEventProfilingInfo(incompleteEvents.front(), CL_PROFILING_COMMAND_START, sizeof(unsigned long long), &started, 0));
            CL_ASSERT(clGetEventProfilingInfo(incompleteEvents.front(), CL_PROFILING_COMMAND_END, sizeof(unsigned long long), &finished, 0));
            framework->getPerformanceReport()->recordAction(incompleteEventCaptions.front(), SingleInstanceTimestamps(queued, submitted, started, finished));
            incompleteEvents.erase(incompleteEvents.begin());
            incompleteEventCaptions.pop_front();
        }
    }

    bool CLAbstractionLayer::HasImageSupport() const
    {
        return imageSupport;
    }
}