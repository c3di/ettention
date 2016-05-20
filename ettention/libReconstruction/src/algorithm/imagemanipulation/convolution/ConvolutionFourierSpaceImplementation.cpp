#include "stdafx.h"
#include "ConvolutionFourierSpaceImplementation.h"
#include "framework/Framework.h"
#include "framework/NumericalAlgorithms.h"
#include "algorithm/imagemanipulation/ComplexMultiplyKernel.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"
#include "algorithm/imagemanipulation/fft/FFTCroppingKernel.h"
#include "algorithm/imagemanipulation/fft/FFTForwardKernel.h"
#include "algorithm/imagemanipulation/fft/FFTPaddingKernel.h"
#include "algorithm/imagemanipulation/PSFShiftingKernel.h"

namespace ettention
{
    ConvolutionFourierSpaceImplementation::ConvolutionFourierSpaceImplementation(Framework* framework, GPUMapped<Image>* filterCandidate, GPUMapped<Image>* sourceCandidate, GPUMapped<Image>* outputContainer, bool kernelRequiresOriginShifting)
        : unifiedResolution(getUnifiedResolution(filterCandidate, sourceCandidate))
        , kernelRequiresOriginShifting(kernelRequiresOriginShifting)
        , filterKernel(filterCandidate)
        , sourceImage(sourceCandidate)
    {
        initKernels(framework);
        cropKernel = new FFTCroppingKernel(framework, backKernel->getOutput(), outputContainer);
    }

    ConvolutionFourierSpaceImplementation::ConvolutionFourierSpaceImplementation(Framework* framework, GPUMapped<Image>* filterCandidate, GPUMapped<Image>* sourceCandidate, bool kernelRequiresOriginShifting)
        : unifiedResolution(getUnifiedResolution(filterCandidate, sourceCandidate))
        , kernelRequiresOriginShifting(kernelRequiresOriginShifting)
        , filterKernel(filterCandidate)
        , sourceImage(sourceCandidate)
    {
        initKernels(framework);
        cropKernel = new FFTCroppingKernel(framework, backKernel->getOutput(), sourceImage->getResolution());
    }

    ConvolutionFourierSpaceImplementation::~ConvolutionFourierSpaceImplementation()
    {
        delete cropKernel;

        delete backKernel;
        delete multiplyKernel;
        delete forwardKernelForSource;
        delete forwardKernelForFilter;
        delete shiftingKernelForFilter;
        delete paddingKernelForSource;
        delete paddingKernelForFilter;
        delete restoreOriginOfFilter;
    }

    void ConvolutionFourierSpaceImplementation::initKernels(Framework* framework)
    {
        restoreOriginOfFilter       = new PSFShiftingKernel(framework, filterKernel);
        paddingKernelForFilter      = new FFTPaddingKernel(framework, restoreOriginOfFilter->getOutput(), unifiedResolution, true);
        shiftingKernelForFilter     = new PSFShiftingKernel(framework, paddingKernelForFilter->getOutput());
        forwardKernelForFilter      = new FFTForwardKernel(framework->getOpenCLStack(), shiftingKernelForFilter->getOutput());

        paddingKernelForSource      = new FFTPaddingKernel(framework, sourceImage, unifiedResolution);
        forwardKernelForSource      = new FFTForwardKernel(framework->getOpenCLStack(), paddingKernelForSource->getOutput());

        multiplyKernel              = new ComplexMultiplyKernel(framework,
                                                                forwardKernelForFilter->getOutputRealPart(),
                                                                forwardKernelForFilter->getOutputImaginaryPart(),
                                                                forwardKernelForSource->getOutputRealPart(),
                                                                forwardKernelForSource->getOutputImaginaryPart());
        backKernel                  = new FFTBackKernel(framework->getOpenCLStack(), multiplyKernel->getRealOutput(), multiplyKernel->getImaginaryOutput());

        if( kernelRequiresOriginShifting )
        {
            paddingKernelForFilter->setInput(filterKernel);
        }
    }

    void ConvolutionFourierSpaceImplementation::execute()
    {
        restoreOriginOfFilter->run();
        paddingKernelForFilter->run();
        shiftingKernelForFilter->run();
        forwardKernelForFilter->run();

        paddingKernelForSource->run();
        forwardKernelForSource->run();

        multiplyKernel->run();
        backKernel->run();
        cropKernel->run();

        cropKernel->getOutput()->ensureIsUpToDateOnCPU();
    }

    void ConvolutionFourierSpaceImplementation::setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting)
    {
        filterKernel = buffer;
        Vec2ui resolution = this->getUnifiedResolution(buffer, this->paddingKernelForSource->getOutput());

        if(requiresOriginShifting)
        {
            paddingKernelForFilter->setInput(buffer);
        } else {
            paddingKernelForFilter->setInput(restoreOriginOfFilter->getOutput());
        }

        paddingKernelForFilter->setDesiredResolution(resolution);
        shiftingKernelForFilter->setInput(paddingKernelForFilter->getOutput());
    }

    void ConvolutionFourierSpaceImplementation::setInput(GPUMapped<Image>* source)
    {
        sourceImage = source;
        Vec2ui resolution = getUnifiedResolution(this->paddingKernelForFilter->getOutput(), source);

        this->paddingKernelForSource->setInput(source);
        paddingKernelForSource->setDesiredResolution(resolution);
        this->forwardKernelForSource->setInput(paddingKernelForSource->getOutput());
    }

    void ConvolutionFourierSpaceImplementation::setOutput(GPUMapped<Image>* buffer)
    {
        cropKernel->setResultContainer(buffer);
    }

    GPUMapped<Image> *ConvolutionFourierSpaceImplementation::getOutput() const
    {
        return cropKernel->getOutput();
    }

    Image *ConvolutionFourierSpaceImplementation::getPaddedFilter()
    {
        paddingKernelForFilter->getOutput()->ensureIsUpToDateOnCPU();
        return paddingKernelForFilter->getOutput()->getObjectOnCPU();
    }

    Image *ConvolutionFourierSpaceImplementation::getPaddedShiftedFilter()
    {
        shiftingKernelForFilter->getOutput()->ensureIsUpToDateOnCPU();
        return shiftingKernelForFilter->getOutput()->getObjectOnCPU();
    }

    Image *ConvolutionFourierSpaceImplementation::getPaddedSource()
    {
        paddingKernelForSource->getOutput()->ensureIsUpToDateOnCPU();
        return paddingKernelForSource->getOutput()->getObjectOnCPU();
    }

    Image *ConvolutionFourierSpaceImplementation::getFullSizeResult()
    {
        backKernel->getOutput()->ensureIsUpToDateOnCPU();
        return backKernel->getOutput()->getObjectOnCPU();
    }

    Vec2ui ConvolutionFourierSpaceImplementation::getUnifiedResolution(GPUMapped<Image>* imageA, GPUMapped<Image>* imageB)
    {
        Vec2ui maxResolution = Vec2ui(std::max(imageA->getResolution().x, imageB->getResolution().x), std::max(imageA->getResolution().y, imageB->getResolution().y));
        Vec2ui maxPoTResolution = NumericalAlgorithms::nearestPowerOfTwo(maxResolution);
        return maxPoTResolution;
    }

    void ConvolutionFourierSpaceImplementation::checkImageIsValidAsItIsOrFail(GPUMapped<Image>* imageA, GPUMapped<Image>* imageB)
    {
        if( imageA->getResolution() != imageB->getResolution() )
        {
            throw Exception("As the limitation of clAMDFFTlib: input images have to be the same in size.");
        }

        Vec2ui imageApowerOfTwoResolution = Vec2ui(NumericalAlgorithms::nearestPowerOfTwo(imageA->getResolution()));
        Vec2ui imageBpowerOfTwoResolution = Vec2ui(NumericalAlgorithms::nearestPowerOfTwo(imageB->getResolution()));

        if( (imageApowerOfTwoResolution != imageA->getResolution())
         || (imageBpowerOfTwoResolution != imageB->getResolution()))
        {
            throw Exception("As the limitation of clAMDFFTlib: input images have to be Power-of-Two in size (or power of three/five, but we do not allow these).");
        }
    }
}