#include "stdafx.h"
#include "CrosscorrelationKernel.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    CrosscorrelationKernel::CrosscorrelationKernel(Framework* framework, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, GPUMapped<Image> *reference, GPUMapped<Image> *candidate, float candidateMean)
        : ComputeKernel(framework, sourceCode, kernelName, kernelCaption)
        , candidateMean(candidateMean)
    {
        setInputs(reference, candidate);
        allocateOutput();
    }

    CrosscorrelationKernel::~CrosscorrelationKernel()
    {
        releaseOutput();
    }

    void CrosscorrelationKernel::allocateOutput()
    {
        result = new GPUMapped<Image>(abstractionLayer, projectionResolution);
    }

    void CrosscorrelationKernel::releaseOutput()
    {
        delete result;
    }

    void CrosscorrelationKernel::prepareKernelArguments()
    {
        implementation->setArgument("reference", reference);
        implementation->setArgument("candidate", candidate);
        implementation->setArgument("result", result);
        implementation->setArgument("referenceSizeX", (int)projectionResolution.x);
        implementation->setArgument("referenceSizeY", (int)projectionResolution.y);
        implementation->setArgument("candidateSizeX", (int)patchResolution.x);
        implementation->setArgument("candidateSizeY", (int)patchResolution.y);
        implementation->setArgument("candidateMean", candidateMean);
    }

    void CrosscorrelationKernel::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(projectionResolution);
        implementation->setGlobalWorkSize(dim);
    }

    void CrosscorrelationKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    void CrosscorrelationKernel::setInputs(GPUMapped<Image>* reference, GPUMapped<Image>* candidate)
    {
        setInputReference(reference);
        setInputCandidate(candidate);
    }

    void CrosscorrelationKernel::setInputReference(GPUMapped<Image>* buffer)
    {
        reference = buffer;
        projectionResolution = buffer->getResolution().xy();
    }

    void CrosscorrelationKernel::setInputCandidate(GPUMapped<Image>* buffer)
    {
        candidate = buffer;
        patchResolution = buffer->getResolution().xy();
    }

    void CrosscorrelationKernel::setCandidateMean(float mean)
    {
        candidateMean = mean;
    }

    GPUMapped<Image>* CrosscorrelationKernel::getResult() const
    {
        return result;
    }
}