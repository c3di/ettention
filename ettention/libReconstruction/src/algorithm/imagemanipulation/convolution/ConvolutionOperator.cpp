#include "stdafx.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperator.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionRealSpaceImplementation.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionFourierSpaceImplementation.h"
#include "framework/Framework.h"
#include "framework/NumericalAlgorithms.h"

namespace ettention
{
    ConvolutionOperator::ConvolutionOperator(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer, bool kernelRequiresOriginShifting)
        : doUseRealSpaceImplementation(shouldUseRealSpace(convolutionKernel, source))
    {
        if( this->doUseRealSpaceImplementation )
        {
            this->implementation = new ConvolutionRealSpaceImplementation(framework, convolutionKernel, source, outputContainer);
        } else {
            this->implementation = new ConvolutionFourierSpaceImplementation(framework, convolutionKernel, source, outputContainer, kernelRequiresOriginShifting);
        }
    }

    ConvolutionOperator::ConvolutionOperator(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, bool kernelRequiresOriginShifting)
        : doUseRealSpaceImplementation(shouldUseRealSpace(convolutionKernel, source))
    {
        if( this->doUseRealSpaceImplementation )
        {
            this->implementation = new ConvolutionRealSpaceImplementation(framework, convolutionKernel, source);
        } else
        {
            this->implementation = new ConvolutionFourierSpaceImplementation(framework, convolutionKernel, source, kernelRequiresOriginShifting);
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

    GPUMapped<Image>* ConvolutionOperator::getOutput() const
    {
        return this->implementation->getOutput();
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

    bool ConvolutionOperator::shouldUseRealSpace(GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source)
    {
        unsigned int largesize = std::max(source->getResolution().x, source->getResolution().y);
        unsigned int smallsize = std::max(convolutionKernel->getResolution().x, convolutionKernel->getResolution().y);
        unsigned int magick    = (unsigned int)(4096 / largesize * 1.2) + 17; // Guessed approximation -_-
        if((largesize >= 128) || (largesize <= 4096))
        {
            if( smallsize < magick )
            {
                return true;
            } else {
                return false;
            }
        } else
        if(smallsize < 128)
        {
            return true;
        }

        return false;
    }

}