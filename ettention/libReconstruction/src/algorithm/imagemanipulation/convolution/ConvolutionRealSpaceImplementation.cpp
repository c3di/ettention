#include "stdafx.h"
#include "ConvolutionRealSpaceImplementation.h"

#include "algorithm/imagemanipulation/convolution/ConvolutionRealSpaceKernel.h"

namespace ettention
{
    ConvolutionRealSpaceImplementation::ConvolutionRealSpaceImplementation(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer)
        : convolutionInRealSpaceCLKernel(new ConvolutionRealSpaceKernel(framework, convolutionKernel, source, outputContainer))
    {
    }

    ConvolutionRealSpaceImplementation::ConvolutionRealSpaceImplementation(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source)
        : convolutionInRealSpaceCLKernel(new ConvolutionRealSpaceKernel(framework, convolutionKernel, source))
    {
    }

    ConvolutionRealSpaceImplementation::~ConvolutionRealSpaceImplementation()
    {
        delete convolutionInRealSpaceCLKernel;
    }

    void ConvolutionRealSpaceImplementation::setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting) // ignore bool parameter (sic!)
    {
        convolutionInRealSpaceCLKernel->setKernel(buffer);
    }

    void ConvolutionRealSpaceImplementation::setInput(GPUMapped<Image>* buffer)
    {
        convolutionInRealSpaceCLKernel->setInput(buffer);
    }

    void ConvolutionRealSpaceImplementation::setOutput(GPUMapped<Image>* buffer)
    {
        convolutionInRealSpaceCLKernel->setResultContainer(buffer);
    }

    GPUMapped<Image>* ConvolutionRealSpaceImplementation::getOutput() const
    {
        return convolutionInRealSpaceCLKernel->getOutput();
    }

    float ConvolutionRealSpaceImplementation::getWeightsSum() const
    {
        return convolutionInRealSpaceCLKernel->getWeightsSum();
    }

    void ConvolutionRealSpaceImplementation::execute()
    {
        convolutionInRealSpaceCLKernel->run();
    }

}