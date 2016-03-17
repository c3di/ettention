#include "stdafx.h"
#include "CrosscorrelationKernel.h"

#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    CrosscorrelationKernel::CrosscorrelationKernel(Framework* framework, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, GPUMapped<Image> *reference, GPUMapped<Image> *candidate, float candidateMean)
        : ImageProcessingKernel(framework, reference->getResolution(), sourceCode, kernelName, kernelCaption)
        , candidateMean(candidateMean)
    {
        setInputs(reference, candidate);
    }

    CrosscorrelationKernel::CrosscorrelationKernel(Framework* framework, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, GPUMapped<Image> *reference, GPUMapped<Image> *candidate, GPUMapped<Image> *outputContainer, float candidateMean)
        : ImageProcessingKernel(framework, outputContainer, sourceCode, kernelName, kernelCaption)
        , candidateMean(candidateMean)
    {
        setInputs(reference, candidate);
    }

    CrosscorrelationKernel::~CrosscorrelationKernel()
    {
    }

    void CrosscorrelationKernel::prepareKernelArguments()
    {
        implementation->setArgument("reference", reference);
        implementation->setArgument("candidate", candidate);
        implementation->setArgument("result", getOutput());
        implementation->setArgument("referenceSizeX", (int)projectionResolution.x);
        implementation->setArgument("referenceSizeY", (int)projectionResolution.y);
        implementation->setArgument("candidateSizeX", (int)patchResolution.x);
        implementation->setArgument("candidateSizeY", (int)patchResolution.y);
        implementation->setArgument("candidateMean", candidateMean);
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
}