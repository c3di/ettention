#include "stdafx.h"
#include "GlobalMinimumPositionInterface.h"
#include "GlobalMinimumPosition_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"
#include <cmath>

namespace ettention
{
    GlobalMinimumPositionInterface::GlobalMinimumPositionInterface(Framework* framework, CLMemBuffer* source, int taskSize)
        : ComputeKernel(framework, GlobalMinimumPosition_kernel_SourceCode, "computeGlobalMinimumPosition", "GlobalMinimumPositionInterface")
        , source(source)
        , taskSize(taskSize)
    {
        implementation->queryAndApplyOptimumWorkSize1D((taskSize / 2) + (taskSize % 2), this->workersTotal, this->workersInWorkgroup);
        this->numberOfWorkgroups = this->workersTotal / this->workersInWorkgroup;
        if( (this->workersTotal % this->workersInWorkgroup) != 0 )
            this->numberOfWorkgroups++;

        createOutputBuffer();
    }

    GlobalMinimumPositionInterface::~GlobalMinimumPositionInterface()
    {
        deleteOutputBuffer();
    }

    void GlobalMinimumPositionInterface::prepareKernelArguments()
    {
        implementation->setArgument("source", source);
        implementation->setArgument("resultValue", resultValues);
        implementation->setArgument("resultPosition", resultPositions);
        implementation->setArgument("taskSize", taskSize);
        implementation->allocateArgument("minValuesBuffer", workersInWorkgroup * sizeof(float));
        implementation->allocateArgument("minPositionsBuffer", workersInWorkgroup * sizeof(int));
    }

    void GlobalMinimumPositionInterface::preRun()
    {
    }

    void GlobalMinimumPositionInterface::postRun()
    {
        resultValues->markAsChangedOnGPU();
        resultPositions->markAsChangedOnGPU();
        resultValues->ensureIsUpToDateOnCPU();
        resultPositions->ensureIsUpToDateOnCPU();

        minimumPosition = resultPositions->getObjectOnCPU()->at(0);
        minimumValue = resultValues->getObjectOnCPU()->at(0);
        float temp;
        for( int i = 1; i < numberOfWorkgroups; ++i )
        {
            temp = resultValues->getObjectOnCPU()->at(i);
            if( (isnan(minimumValue)) || ( (!isnan(temp)) && ( temp < minimumValue ) ) )
            {
                minimumPosition = resultPositions->getObjectOnCPU()->at(i);
                minimumValue = resultValues->getObjectOnCPU()->at(i);
            }
        }
    }

    int GlobalMinimumPositionInterface::getMinimumPosition() const
    {
        return minimumPosition;
    }

    float GlobalMinimumPositionInterface::getMinimumValue() const
    {
        return minimumValue;
    }

    void GlobalMinimumPositionInterface::createOutputBuffer()
    {
        resultValues = new GPUMappedVector(abstractionLayer, numberOfWorkgroups);
        resultPositions = new GPUMappedIntVector(abstractionLayer, numberOfWorkgroups);
    }

    void GlobalMinimumPositionInterface::deleteOutputBuffer()
    {
        delete resultPositions;
        delete resultValues;
    }
}