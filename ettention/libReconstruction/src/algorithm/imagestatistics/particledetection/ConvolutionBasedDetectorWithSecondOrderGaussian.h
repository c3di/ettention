#pragma once
#include "algorithm/imagemanipulation/CaoDetectorKernel.h"
#include "algorithm/imagemanipulation/GenerateGausssian2ordKernel.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"
#include "algorithm/imagestatistics/EigenvaluesOfGaussianKernel.h"
#include "algorithm/imagestatistics/localmaximum/LocalMaximumSimpleKernel.h"
#include "algorithm/imagestatistics/particledetection/ConvolutionBasedDetector.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class ConvolutionOperator;
    class Image;
    template<typename _T> class GPUMapped;

    class ConvolutionBasedDetectorWithSecondOrderGaussian : public ConvolutionBasedDetector
    {
    public:
        ConvolutionBasedDetectorWithSecondOrderGaussian(Framework *framework, GPUMapped<Image> *source, unsigned int particleDiameter);
        ConvolutionBasedDetectorWithSecondOrderGaussian(Framework *framework, GPUMapped<Image> *source, unsigned int particleDiameter, unsigned int gaussianDiameter);
        virtual ~ConvolutionBasedDetectorWithSecondOrderGaussian();

        virtual void process(GPUMapped<Image> *source) override;
        virtual void process() override;
        virtual void adjustThreshold(float value) override;
        
        float getScale();
        void doDebugOutput(std::string pathPrefix);

        // To bypass cases, when source takes to much GPU memory and cannot being processed in pipeline with lots of copies.
        void lowerResourceProcess(GPUMapped<Image> *convolInXXDir, GPUMapped<Image> *convolInXYDir, GPUMapped<Image> *convolInYYDir); 

    protected:
        void init(Framework *framework);
        void preComputeDirectionalGaussian();
        void preComputeConvolutionWithInput();
        void initRemainingKernels(Framework *framework);
        void clearPreComputedConvolution();

    private:
        GenerateGausssian2ordKernel *gaussianKernel;            // Init/Release in Ctor/Dtor
        ConvolutionOperator *convolutionOperator;
        EigenvaluesOfGaussianKernel *eigenSolveKernel;
        CaoDetectorKernel *caoKernel;
        ThresholdKernel *thresholdKernel;
        LocalMaximumSimpleKernel *nmsKernel;

        Vec2ui resolution;

        GPUMapped<Image> *source;                               // Temp object to save input object as tif image
        GPUMapped<Image> *sourceConvXX;                         // Source convoluted with 2nd order Gaussian X-X, X-Y and Y-Y directions
        GPUMapped<Image> *sourceConvXY;
        GPUMapped<Image> *sourceConvYY;
    };
}
