#include "stdafx.h"
#include "ConvolutionImplementation.h"

namespace ettention 
{

    ConvolutionImplementation::ConvolutionImplementation()
    {
    }

    ConvolutionImplementation::~ConvolutionImplementation()
    {
    }

    void ConvolutionImplementation::setInputs(GPUMapped<Image>* kernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting)
    {
        this->setKernel(kernel, kernelRequiresOriginShifting);
        this->setInput(source);
    }
}