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
    ConvolutionFourierSpaceImplementation::ConvolutionFourierSpaceImplementation(Framework* framework, GPUMapped<Image>* filterCandidate, GPUMapped<Image>* sourceCandidate, bool kernelRequiresOriginShifting)
        : abstractionLayer(framework->getOpenCLStack())
        , filterKernel(filterCandidate), sourceImage(sourceCandidate)
    {
        Vec2ui resolution = this->getUnifiedResolution(filterCandidate, sourceCandidate);

        restoreOriginOfFilter = new PSFShiftingKernel(framework, filterCandidate);
        paddingKernelForFilter = new FFTPaddingKernel(framework, restoreOriginOfFilter->getResult(), resolution, true);
        shiftingKernelForFilter = new PSFShiftingKernel(framework, paddingKernelForFilter->getResult());
        forwardKernelForFilter = new FFTForwardKernel(abstractionLayer, shiftingKernelForFilter->getResult());

        paddingKernelForSource = new FFTPaddingKernel(framework, sourceCandidate, resolution);
        forwardKernelForSource = new FFTForwardKernel(abstractionLayer, paddingKernelForSource->getResult());

        multiplyKernel = new ComplexMultiplyKernel(framework,
                                                   forwardKernelForFilter->getOutputRealPart(),
                                                   forwardKernelForFilter->getOutputImaginaryPart(),
                                                   forwardKernelForSource->getOutputRealPart(),
                                                   forwardKernelForSource->getOutputImaginaryPart());
        backKernel = new FFTBackKernel(abstractionLayer, multiplyKernel->getRealOutput(), multiplyKernel->getImaginaryOutput());
        cropKernel = new FFTCroppingKernel(framework, backKernel->getOutput(), sourceCandidate->getResolution());

        if(kernelRequiresOriginShifting)
        {
            this->paddingKernelForFilter->setInput(filterCandidate);
        }
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
    }

    Image *ConvolutionFourierSpaceImplementation::getPaddedFilter()
    {
        paddingKernelForFilter->getResult()->ensureIsUpToDateOnCPU();
        return paddingKernelForFilter->getResult()->getObjectOnCPU();
    }

    Image *ConvolutionFourierSpaceImplementation::getPaddedShiftedFilter()
    {
        shiftingKernelForFilter->getResult()->ensureIsUpToDateOnCPU();
        return shiftingKernelForFilter->getResult()->getObjectOnCPU();
    }

    Image *ConvolutionFourierSpaceImplementation::getPaddedSource()
    {
        paddingKernelForSource->getResult()->ensureIsUpToDateOnCPU();
        return paddingKernelForSource->getResult()->getObjectOnCPU();
    }

    Image *ConvolutionFourierSpaceImplementation::getFullSizeResult()
    {
        backKernel->getOutput()->ensureIsUpToDateOnCPU();
        return backKernel->getOutput()->getObjectOnCPU();
    }

    GPUMapped<Image> *ConvolutionFourierSpaceImplementation::getResult()
    {
        return cropKernel->getResult();
    }

    void ConvolutionFourierSpaceImplementation::setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting)
    {
        filterKernel = buffer;
        Vec2ui resolution = this->getUnifiedResolution(buffer, this->paddingKernelForSource->getResult());

        if(requiresOriginShifting)
        {
            paddingKernelForFilter->setInput(buffer);
        }
        else {
            paddingKernelForFilter->setInput(restoreOriginOfFilter->getResult());
        }
        paddingKernelForFilter->setDesiredResolution(resolution);
        shiftingKernelForFilter->setInput(paddingKernelForFilter->getResult());
    }

    void ConvolutionFourierSpaceImplementation::setInput(GPUMapped<Image>* source)
    {
        sourceImage = source;
        Vec2ui resolution = getUnifiedResolution(this->paddingKernelForFilter->getResult(), source);

        this->paddingKernelForSource->setInput(source);
        paddingKernelForSource->setDesiredResolution(resolution);
        this->forwardKernelForSource->setInput(paddingKernelForSource->getResult());
    }

    void ConvolutionFourierSpaceImplementation::setOutput(GPUMapped<Image>* buffer)
    {
        cropKernel->setOutput(buffer);
    }

    Vec2ui ConvolutionFourierSpaceImplementation::getUnifiedResolution(GPUMapped<Image>* imageA, GPUMapped<Image>* imageB)
    {
        Vec2ui maxResolution = Vec2ui(std::max(imageA->getResolution().x, imageB->getResolution().x), std::max(imageA->getResolution().y, imageB->getResolution().y));
        Vec2ui maxPoTResolution = NumericalAlgorithms::nearestPowerOfTwo(maxResolution);
        return maxPoTResolution;
    }

    void ConvolutionFourierSpaceImplementation::checkImageIsValidAsItIsOrFail(GPUMapped<Image>* imageA, GPUMapped<Image>* imageB)
    {
        if(imageA->getResolution() != imageB->getResolution())
        {
            throw Exception("As the limitation of clAMDFFTlib: input images have to be the same in size.");
        }

        Vec2ui imageApowerOfTwoResolution = Vec2ui(NumericalAlgorithms::nearestPowerOfTwo(imageA->getResolution()));
        Vec2ui imageBpowerOfTwoResolution = Vec2ui(NumericalAlgorithms::nearestPowerOfTwo(imageB->getResolution()));

        if((imageApowerOfTwoResolution != imageA->getResolution())
           || (imageBpowerOfTwoResolution != imageB->getResolution()))
        {
            throw Exception("As the limitation of clAMDFFTlib: input images have to be Power-of-Two in size (or power of three/five, but we do not allow these).");
        }
    }
}