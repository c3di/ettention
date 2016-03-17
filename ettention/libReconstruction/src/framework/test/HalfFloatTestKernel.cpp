#include "stdafx.h"
#include "HalfFloatTestKernel.h"
#include "HalfFloatTest_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    HalfFloatTestKernel::HalfFloatTestKernel(Framework* framework, GPUMappedUShortVector* data)
        : ComputeKernel(framework, HalfFloatTest_kernel_SourceCode, "halfFloatTest", "HalfFloatTestKernel")
        , data(data)
    {
    }

    HalfFloatTestKernel::~HalfFloatTestKernel()
    {
    }

    void HalfFloatTestKernel::prepareKernelArguments()
    {
        implementation->setArgument("data", data);
    }

    void HalfFloatTestKernel::preRun()
    {
        implementation->setGlobalWorkSize(1);
    }

    void HalfFloatTestKernel::postRun()
    {
        data->markAsChangedOnGPU();
    }
}