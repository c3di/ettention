#pragma once
#include "algorithm/imagemanipulation/convolution/ConvolutionOperatorImplementation.h"

namespace ettention
{
    class ComplexMultiplyKernel;
    class FFTBackKernel;
    class FFTCroppingKernel;
    class FFTForwardKernel;
    class FFTPaddingKernel;
    class Framework;
    class PSFShiftingKernel;

    class ConvolutionFourierSpaceImplementation : public ConvolutionOperatorImplementation
    {
    public:
        ConvolutionFourierSpaceImplementation(Framework* framework, GPUMapped<Image>* filterCandidate, GPUMapped<Image>* sourceCandidate, bool kernelRequiresOriginShifting);
        virtual ~ConvolutionFourierSpaceImplementation();

        virtual void execute() override;

        virtual void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting);
        virtual void setInput(GPUMapped<Image>* sourceImage);
        virtual void setOutput(GPUMapped<Image>* sourceImage);

        Image *getPaddedFilter();
        Image *getPaddedShiftedFilter();
        Image *getPaddedSource();
        Image *getFullSizeResult();
        virtual GPUMapped<Image>* getResult();

    protected:
        Vec2ui getUnifiedResolution(GPUMapped<Image>* imageA, GPUMapped<Image>* imageB);
        void checkImageIsValidAsItIsOrFail(GPUMapped<Image>* imageA, GPUMapped<Image> *imageB);

    protected:
        CLAbstractionLayer* abstractionLayer;
        GPUMapped<Image>* filterKernel;
        GPUMapped<Image>* sourceImage;

        PSFShiftingKernel* restoreOriginOfFilter;
        FFTPaddingKernel* paddingKernelForFilter;
        PSFShiftingKernel* shiftingKernelForFilter;
        FFTForwardKernel* forwardKernelForFilter;

        FFTPaddingKernel* paddingKernelForSource;
        FFTForwardKernel* forwardKernelForSource;

        ComplexMultiplyKernel* multiplyKernel;
        FFTBackKernel* backKernel;
        FFTCroppingKernel* cropKernel;
    };
}

