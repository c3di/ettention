#include "stdafx.h"
#include "GenerateRamLakFilterKernel.h"
#include "GenerateRamLakKernel_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    GenerateRamLakKernel::GenerateRamLakKernel(Framework* framework, const unsigned int resolution)
        : ComputeKernel(framework, GenerateRamLakKernel_kernel_SourceCode, "generateKernel", "GenerateRamLakKernel")
        , resolution(resolution)
    {
        result = new GPUMapped<Image>(abstractionLayer, Vec2ui(resolution, 1));
    }

    GenerateRamLakKernel::~GenerateRamLakKernel()
    {
        delete result;
    }

    void GenerateRamLakKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", resolution);
        implementation->setArgument("kernelValues", result);
    }

    void GenerateRamLakKernel::preRun()
    {
        implementation->setGlobalWorkSize(Vec2ui(resolution, 1));
    }

    void GenerateRamLakKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    GPUMapped<Image> *GenerateRamLakKernel::getOutput()
    {
        return result;
    }
}