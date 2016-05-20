#include "stdafx.h"
#include "CompareKernel.h"
#include "Compare_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    CompareKernel::CompareKernel(Framework* framework, const Vec2ui& projectionResolution, GPUMapped<Image>* realProjection, GPUMapped<Image>* virtualProjection)
        : ComputeKernel(framework, Compare_kernel_SourceCode, "compare", "CompareKernel")
        , projectionResolution(projectionResolution)
        , realProjection(realProjection)
        , virtualProjection(virtualProjection)
        , ownsResiduals(true)
    {
        residuals = new GPUMapped<Image>((CLAbstractionLayer*)abstractionLayer, projectionResolution);
    }

    CompareKernel::~CompareKernel()
    {
        if(ownsResiduals)
        {
            delete residuals;
        }
    }

    void CompareKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", projectionResolution);
        realProjection->ensureIsUpToDateOnGPU();
        implementation->setArgument("realProjection", realProjection);
        virtualProjection->ensureIsUpToDateOnGPU();
        implementation->setArgument("virtualProjection", virtualProjection);
        implementation->setArgument("residual", residuals);
    }

    void CompareKernel::preRun()
    {
        implementation->setGlobalWorkSize(projectionResolution);
    }

    void CompareKernel::postRun()
    {
        residuals->markAsChangedOnGPU();
    }

    GPUMapped<Image>* CompareKernel::getOutput()
    {
        return residuals;
    }

    void CompareKernel::setInput(GPUMapped<Image>* realProjection, GPUMapped<Image>* virtualProjection)
    {
        this->realProjection = realProjection;
        this->virtualProjection = virtualProjection;
    }

    void CompareKernel::setOutput(GPUMapped<Image>* residuals)
    {
        if(ownsResiduals)
        {
            delete this->residuals;
            ownsResiduals = false;
        }
        this->residuals = residuals;
    }
}