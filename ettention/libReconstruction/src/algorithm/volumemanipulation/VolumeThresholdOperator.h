#pragma once
#include "gpu/GPUMapped.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class Framework;
    class VolumeThresholdKernel;

    class VolumeThresholdOperator
    {
    public:
        VolumeThresholdOperator(Framework* framework, GPUMappedVolume* sourceVolume, float thresholdFloorLevel, GPUMappedVolume* maskVolume = nullptr);
        VolumeThresholdOperator(Framework* framework, GPUMappedVolume* sourceVolume, std::vector<float> *floorLevels, std::vector<float> *floorValues, GPUMappedVolume* maskVolume = nullptr);
        ~VolumeThresholdOperator();

        void run();
        void setInput(GPUMappedVolume *sourceVolume);
        GPUMappedVolume* getOutput() const;

    private:
        void allocateMemory(Framework *framework, std::vector<float> *floorLevels, std::vector<float> *floorValues);
        void freeMemory();

    private:
        GPUMappedVolume* inputVolume;
        GPUMappedVolume* outputVolume;
        GPUMappedVolume* maskVolume;
        GPUMappedVector* thresholdFloorLevels;
        GPUMappedVector* thresholdFloorValues;

        VolumeThresholdKernel* thresholdKernel;
    };
}