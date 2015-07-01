#include "stdafx.h"
#include "CaoDetectorKernel.h"
#include "DetectorCao_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    CaoDetectorKernel::CaoDetectorKernel(Framework* framework, GPUMapped<Image>* minLambdas, GPUMapped<Image>* maxLambdas)
        : ComputeKernel(framework, DetectorCao_kernel_SourceCode, "computeDetectorCao", "CaoDetectorKernel")
        , projectionResolution(minLambdas->getResolution().xy())
    {
        setInput(minLambdas, maxLambdas);
        createOutputBuffer();
    }

    CaoDetectorKernel::~CaoDetectorKernel()
    {
        deleteOutputBuffer();
    }

    void CaoDetectorKernel::prepareKernelArguments()
    {
        implementation->setArgument("minLambda", mins);
        implementation->setArgument("maxLambda", maxs);
        implementation->setArgument("D", result);
        implementation->setArgument("sizeX", (int)projectionResolution.x);
        implementation->setArgument("sizeY", (int)projectionResolution.y);
    }

    void CaoDetectorKernel::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(projectionResolution);
        implementation->setGlobalWorkSize(dim);
    }

    void CaoDetectorKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    void CaoDetectorKernel::createOutputBuffer()
    {
        result = new GPUMapped<Image>(abstractionLayer, projectionResolution);
    }

    void CaoDetectorKernel::deleteOutputBuffer()
    {
        delete result;
    }

    void CaoDetectorKernel::setInput(GPUMapped<Image>* mins, GPUMapped<Image>* maxs)
    {
        if(mins->getResolution().xy() != projectionResolution)
            throw ImageResolutionDiffersException(this->projectionResolution, mins->getResolution().xy(), "CaoDetectorKernel::setInput (min)");

        if(maxs->getResolution().xy() != projectionResolution)
            throw ImageResolutionDiffersException(this->projectionResolution, maxs->getResolution().xy(), "CaoDetectorKernel::setInput (max)");

        this->mins = mins;
        this->maxs = maxs;
    }

    GPUMapped<Image>* CaoDetectorKernel::getResult()
    {
        return this->result;
    }

    Image* CaoDetectorKernel::getResultAsImage()
    {
        this->result->ensureIsUpToDateOnCPU();
        return this->result->getObjectOnCPU();
    }
}