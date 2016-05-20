#include "stdafx.h"
#include "GlobalMaximumPositionInterface.h"
#include "GlobalMaximumPosition_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"
#include <cmath>

namespace ettention
{
    GlobalMaximumPositionInterface::GlobalMaximumPositionInterface(Framework* framework, CLMemBuffer* source, int taskSize)
        : ComputeKernel(framework, GlobalMaximumPosition_kernel_SourceCode, "computeGlobalMaximumPosition", "GlobalMaximumPositionInterface")
        , source(source)
        , taskSize(taskSize)
    {
        implementation->queryAndApplyOptimumWorkSize1D((taskSize / 2) + (taskSize % 2), this->workersTotal, this->workersInWorkgroup);
        this->numberOfWorkgroups = this->workersTotal / this->workersInWorkgroup;
        if((this->workersTotal % this->workersInWorkgroup) != 0)
            this->numberOfWorkgroups++;

        createOutputBuffer();
    }

    GlobalMaximumPositionInterface::~GlobalMaximumPositionInterface()
    {
        deleteOutputBuffer();
    }

    void GlobalMaximumPositionInterface::prepareKernelArguments()
    {
        implementation->setArgument("source", source);
        implementation->setArgument("resultValue", resultValues);
        implementation->setArgument("resultPosition", resultPositions);
        implementation->setArgument("taskSize", taskSize);
        implementation->allocateArgument("maxValuesBuffer", workersInWorkgroup * sizeof(float));
        implementation->allocateArgument("maxPositionsBuffer", workersInWorkgroup * sizeof(int));
    }

    void GlobalMaximumPositionInterface::preRun()
    {
    }

    void GlobalMaximumPositionInterface::postRun()
    {
        resultValues->markAsChangedOnGPU();
        resultPositions->markAsChangedOnGPU();
        resultValues->ensureIsUpToDateOnCPU();
        resultPositions->ensureIsUpToDateOnCPU();

        maximumPosition = resultPositions->getObjectOnCPU()->at(0);
        maximumValue = resultValues->getObjectOnCPU()->at(0);
        float temp;
        for( int i = 1; i < numberOfWorkgroups; ++i )
        {
            temp = resultValues->getObjectOnCPU()->at(i);
            if( (isnan(maximumValue)) || ( (!isnan(temp)) && ( temp > maximumValue ) ) )
            {
                maximumPosition = resultPositions->getObjectOnCPU()->at(i);
                maximumValue = resultValues->getObjectOnCPU()->at(i);
            }
        }
    }

    int GlobalMaximumPositionInterface::getMaximumPosition() const
    {
        return maximumPosition;
    }

    float GlobalMaximumPositionInterface::getMaximumValue() const
    {
        return maximumValue;
    }

    void GlobalMaximumPositionInterface::createOutputBuffer()
    {
        resultValues = new GPUMappedVector(abstractionLayer, numberOfWorkgroups);
        resultPositions = new GPUMappedIntVector(abstractionLayer, numberOfWorkgroups);
    }

    void GlobalMaximumPositionInterface::deleteOutputBuffer()
    {
        delete resultPositions;
        delete resultValues;
    }
}