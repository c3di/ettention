#include "stdafx.h"

#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"

namespace ettention 
{

    ConvolutionOperatorImplementation::ConvolutionOperatorImplementation()
    {
    }

    ConvolutionOperatorImplementation::~ConvolutionOperatorImplementation()
    {
    }

    void ConvolutionOperatorImplementation::setInputs(GPUMapped<Image>* kernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting)
    {
        this->setKernel(kernel, kernelRequiresOriginShifting);
        this->setInput(source);
    }

    Image *ConvolutionOperatorImplementation::getResultAsImage()
    {
        this->getResult()->ensureIsUpToDateOnCPU();
        return this->getResult()->getObjectOnCPU();
    }

    GPUMapped<Image>* ConvolutionOperatorImplementation::getOutput()
    {
        return this->getResult();
    }

}