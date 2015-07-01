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

        virtual Vec2ui getParticleSize() override;

        virtual bool darkFieldSupported() override;

        void initKernels(Framework *framework);
        float getScale();

        virtual void lowerResourceProcess(GPUMapped<Image> *convolInXXDir, GPUMapped<Image> *convolInXYDir, GPUMapped<Image> *convolInYYDir); // To bypass cases, when Convolution takes to much GPU memory and cannot being run in pipeline
        void doDebugOutput(std::string pathPrefix);

    protected:
        void preComputeDirectionalGaussian();

    private:
        GenerateGausssian2ordKernel *gaussianKernel;            // Init/Release in Ctor/Dtor
        ConvolutionOperator *sourceConvXX;                      // Source convoluted with 2nd order Gaussian X-X, X-Y and Y-Y directions
        ConvolutionOperator *sourceConvXY;
        ConvolutionOperator *sourceConvYY;
        EigenvaluesOfGaussianKernel *eigenSolveKernel;
        CaoDetectorKernel *caoKernel;
        ThresholdKernel *thresholdKernel;
        LocalMaximumSimpleKernel *nmsKernel;

        Vec2ui resolution;
        Vec2ui particleSize;

        GPUMapped<Image> *temporarySource;                      // Temp object to save input object as tif image
    };
}
