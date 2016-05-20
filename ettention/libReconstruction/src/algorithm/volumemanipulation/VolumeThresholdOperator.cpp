#include "stdafx.h"
#include "VolumeThresholdOperator.h"

#include "algorithm/volumemanipulation/VolumeThresholdKernel.h"
#include "framework/Framework.h"
#include "framework/error/VolumeResolutionDiffersException.h"

namespace ettention
{
    VolumeThresholdOperator::VolumeThresholdOperator(Framework* framework, GPUMappedVolume* sourceVolume, float thresholdFloorLevel, GPUMappedVolume* maskVolume /*= nullptr*/)
        : inputVolume(sourceVolume)
        , maskVolume(maskVolume)
    {
        std::vector<float> tempLevels(1);
        tempLevels[0] = thresholdFloorLevel;

        std::vector<float> tempValues(1);
        tempValues[0] = 0.0f;

        allocateMemory(framework, &tempLevels, &tempValues);
    }

    VolumeThresholdOperator::VolumeThresholdOperator(Framework* framework, GPUMappedVolume* sourceVolume, std::vector<float> *floorLevels, std::vector<float> *floorValues, GPUMappedVolume* maskVolume /*= nullptr*/)
        : inputVolume(sourceVolume)
        , maskVolume(maskVolume)
    {
        allocateMemory(framework, floorLevels, floorValues);
    }

    VolumeThresholdOperator::~VolumeThresholdOperator()
    {
        freeMemory();
    }

    void VolumeThresholdOperator::allocateMemory(Framework *framework, std::vector<float> *floorLevels, std::vector<float> *floorValues)
    {
        thresholdFloorLevels    = new GPUMappedVector(framework->getOpenCLStack(), new VectorAdaptor<float>(*floorLevels), false);
        thresholdFloorValues    = new GPUMappedVector(framework->getOpenCLStack(), new VectorAdaptor<float>(*floorValues), false);
        outputVolume            = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(inputVolume->getProperties().getVolumeResolution(), 0.0f, inputVolume->getProperties().getSubVolumeCount()), true);

        thresholdFloorLevels->takeOwnershipOfObjectOnCPU();
        thresholdFloorValues->takeOwnershipOfObjectOnCPU();
        outputVolume->takeOwnershipOfObjectOnCPU();

        thresholdFloorLevels->markAsChangedOnCPU();
        thresholdFloorValues->markAsChangedOnCPU();

        thresholdKernel         = new VolumeThresholdKernel(framework, inputVolume, outputVolume, thresholdFloorLevels, thresholdFloorValues, maskVolume);
    }

    void VolumeThresholdOperator::freeMemory()
    {
        delete thresholdKernel;
        delete outputVolume;
        delete thresholdFloorValues;
        delete thresholdFloorLevels;
    }

    void VolumeThresholdOperator::run()
    {
        inputVolume->ensureIsUpToDateOnGPU();
        for( unsigned int i = 0; i < inputVolume->getProperties().getSubVolumeCount(); ++i )
        {
            inputVolume->setCurrentSubvolumeIndex(i);
            outputVolume->setCurrentSubvolumeIndex(i);

            thresholdKernel->run();
        }
    }

    void VolumeThresholdOperator::setInput(GPUMappedVolume *sourceVolume)
    {
        if( this->inputVolume->getProperties().getVolumeResolution() != sourceVolume->getProperties().getVolumeResolution() )
            throw VolumeResolutionDiffersException(this->inputVolume->getProperties().getVolumeResolution(), sourceVolume->getProperties().getVolumeResolution(), "VolumeThresholdOperator::setInput");

        this->inputVolume = sourceVolume;
        thresholdKernel->setInput(sourceVolume);
    }

    GPUMappedVolume* VolumeThresholdOperator::getOutput() const
    {
        return outputVolume;
    }
}