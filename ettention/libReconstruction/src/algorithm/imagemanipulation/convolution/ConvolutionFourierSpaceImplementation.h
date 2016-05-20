#pragma once
#include "algorithm/imagemanipulation/convolution/ConvolutionImplementation.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class Framework;
    class Image;
    template<typename _T> class GPUMapped;

    class ComplexMultiplyKernel;
    class FFTBackKernel;
    class FFTCroppingKernel;
    class FFTForwardKernel;
    class FFTPaddingKernel;
    class PSFShiftingKernel;

    class ConvolutionFourierSpaceImplementation : public ConvolutionImplementation
    {
    public:
        ConvolutionFourierSpaceImplementation(Framework* framework, GPUMapped<Image>* filterCandidate, GPUMapped<Image>* sourceCandidate, GPUMapped<Image>* outputContainer, bool kernelRequiresOriginShifting);
        ConvolutionFourierSpaceImplementation(Framework* framework, GPUMapped<Image>* filterCandidate, GPUMapped<Image>* sourceCandidate, bool kernelRequiresOriginShifting);
        virtual ~ConvolutionFourierSpaceImplementation();

        virtual void execute() override;

        virtual void setKernel(GPUMapped<Image>* buffer, bool requiresOriginShifting) override;
        virtual void setInput(GPUMapped<Image>* sourceImage) override;
        virtual void setOutput(GPUMapped<Image>* buffer) override;

        virtual GPUMapped<Image>* getOutput() const override;

        Image *getPaddedFilter();
        Image *getPaddedShiftedFilter();
        Image *getPaddedSource();
        Image *getFullSizeResult();

    protected:
        void initKernels(Framework* framework);
        Vec2ui getUnifiedResolution(GPUMapped<Image>* imageA, GPUMapped<Image>* imageB);
        void checkImageIsValidAsItIsOrFail(GPUMapped<Image>* imageA, GPUMapped<Image> *imageB);

    protected:
        Vec2ui unifiedResolution;
        bool kernelRequiresOriginShifting;

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

        FFTCroppingKernel* cropKernel; // Cropping kernel is the last kernel in pipeline so it is responsible for result container management
    };
}