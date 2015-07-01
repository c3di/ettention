#include "stdafx.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperator.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionRealSpaceImplementation.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionFourierSpaceImplementation.h"
#include "framework/Framework.h"
#include "framework/NumericalAlgorithms.h"

namespace ettention
{
    ConvolutionOperator::ConvolutionOperator(Framework* framework, GPUMapped<Image>* kernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting)
        : abstractionLayer(framework->getOpenCLStack())
        , doUseRealSpaceImplementation(false)
    {
        doUseRealSpaceImplementation = shouldUseRealSpace(kernel, source);
        if(this->doUseRealSpaceImplementation)
        {
            this->implementation = new ConvolutionRealSpaceImplementation(framework, kernel, source);
        }
        else
        {
            this->implementation = new ConvolutionFourierSpaceImplementation(framework, kernel, source, kernelRequiresOriginShifting);
        }
    }

    ConvolutionOperator::~ConvolutionOperator()
    {
        delete this->implementation;
    }

    void ConvolutionOperator::execute()
    {
        this->implementation->execute();
    }

    GPUMapped<Image>* ConvolutionOperator::getResult()
    {
        return this->implementation->getResult();
    }

    void ConvolutionOperator::setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting)
    {
        this->implementation->setKernel(buffer, requiresOriginShifting);
    }

    void ConvolutionOperator::setInput(GPUMapped<Image>* source)
    {
        implementation->setInput(source);
    }

    void ConvolutionOperator::setOutput(GPUMapped<Image>* buffer)
    {
        implementation->setOutput(buffer);
    }

    bool ConvolutionOperator::shouldUseRealSpace(GPUMapped<Image>* kernel, GPUMapped<Image>* source)
    {
        unsigned int largesize = std::max(source->getResolution().x, source->getResolution().y);
        unsigned int smallsize = std::max(kernel->getResolution().x, kernel->getResolution().y);
        if((largesize >= 128) || (largesize <= 4096))
        {
            if(smallsize < ((4096 / largesize) * 1.2 + 17)) // Guessed approximation -_-
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        if(smallsize < 128)
        {
            return true;
        }

        return false;
    }

}