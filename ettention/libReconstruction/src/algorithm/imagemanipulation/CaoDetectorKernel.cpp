#include "stdafx.h"
#include "CaoDetectorKernel.h"
#include "DetectorCao_bin2c.h"

#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    CaoDetectorKernel::CaoDetectorKernel(Framework* framework, GPUMapped<Image>* minLambdas, GPUMapped<Image>* maxLambdas, GPUMapped<Image>* result)
        : ImageProcessingKernel(framework, result, DetectorCao_kernel_SourceCode, "computeDetectorCao", "CaoDetectorKernel")
        , projectionResolution(minLambdas->getResolution().xy())
    {
        setInput(minLambdas, maxLambdas);
    }

    CaoDetectorKernel::CaoDetectorKernel(Framework* framework, GPUMapped<Image>* minLambdas, GPUMapped<Image>* maxLambdas)
        : ImageProcessingKernel(framework, minLambdas->getResolution(), DetectorCao_kernel_SourceCode, "computeDetectorCao", "CaoDetectorKernel")
        , projectionResolution(minLambdas->getResolution().xy())
    {
        setInput(minLambdas, maxLambdas);
    }

    CaoDetectorKernel::~CaoDetectorKernel()
    {
    }

    void CaoDetectorKernel::prepareKernelArguments()
    {
        implementation->setArgument("minLambda", mins);
        implementation->setArgument("maxLambda", maxs);
        implementation->setArgument("D", getOutput());
        implementation->setArgument("sizeX", (int)projectionResolution.x);
        implementation->setArgument("sizeY", (int)projectionResolution.y);
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
}