#pragma once
#include <vector>
#include <unordered_set>
#include "gpu/opencl/OpenCLHeader.h"
#include "gpu/opencl/CLContextCreator.h"

namespace ettention
{
    class Framework;

    class CLAbstractionLayer
    {
    public:
        CLAbstractionLayer(Framework* framework);
        ~CLAbstractionLayer();

        std::size_t getRemainingMemorySizeInBytes();
        std::size_t getMaxSizeOfIndividualMemoryObjectInBytes();

        cl_device_id getDeviceId();
        cl_context getContext();
        cl_command_queue getCommandQueue();

        void profileAndReleaseEvent(const std::string& eventCaption, cl_event evt);
        bool HasImageSupport() const;

        void allocate(std::size_t byteWidth);
        void deallocate(std::size_t byteWidth);
        void registerCLSampler(cl_sampler clSampler);
        void unregisterCLSampler(cl_sampler clSampler);
        void registerCLMem(cl_mem clMem);
        void unregisterCLMem(cl_mem clMem);

    private:
        Framework* framework;
        cl_command_queue queue;
        cl_context context;
        std::vector<cl_device_id> devices;
        std::unordered_set<cl_mem> aliveCLMemObjects;
        std::unordered_set<cl_sampler> aliveCLSamplers;
        bool isProfilingEnabled;
        CLContextCreator contextCreator;
        bool imageSupport;
        std::size_t remainingMemoryInBytes;
        std::vector<cl_event> incompleteEvents;
        std::list<std::string> incompleteEventCaptions;

        CLAbstractionLayer(const CLAbstractionLayer& other) = delete;
        const CLAbstractionLayer& operator=(const CLAbstractionLayer& other) = delete;

        template <typename T>
        T  getDeviceInfo(cl_device_id devID, cl_device_info devInfo);

        void createCommandQueue();
        void listDevices();
        void printSingleDeviceInfo(cl_device_id deviceId);
        void getMemoryInformationOnDevice(cl_device_id deviceId);
        void checkImageSupport(bool forceDisable);
        void profileCachedEvents();
    };
}