#include "stdafx.h"

#include "ConvolutionBasedDetectorWithSecondOrderGaussian.h"
#include "algorithm/imagestatistics/particledetection/ParticleExtractor.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionOperator.h"
#include "algorithm/imagemanipulation/convolution/ConvolutionFourierSpaceImplementation.h"
#include "algorithm/imagemanipulation/ThresholdKernel.h"
#include "algorithm/imagestatistics/ThresholdDecider.h"
#include "framework/Framework.h"
#include "framework/NumericalAlgorithms.h"
#include "io/serializer/ImageSerializer.h"
#include <cmath>

namespace ettention
{
    ConvolutionBasedDetectorWithSecondOrderGaussian::ConvolutionBasedDetectorWithSecondOrderGaussian(Framework *framework, GPUMapped<Image> *source, unsigned int particleDiameter)
        : ConvolutionBasedDetector(framework->getOpenCLStack())
        , resolution(source->getResolution())
        , temporarySource(source)
        , particleSize(Vec2ui(particleDiameter, particleDiameter))
    {
        operatingInBrightFieldMode = true;

        this->gaussianKernel = new GenerateGausssian2ordKernel(framework, particleDiameter);
        this->gaussianKernel->run();

        this->initKernels(framework);
    }

    ConvolutionBasedDetectorWithSecondOrderGaussian::ConvolutionBasedDetectorWithSecondOrderGaussian(Framework *framework, GPUMapped<Image> *source, unsigned int particleDiameter, unsigned int gaussianDiameter)
        : ConvolutionBasedDetector(framework->getOpenCLStack())
        , resolution(source->getResolution())
        , temporarySource(source)
        , particleSize(Vec2ui(particleDiameter, particleDiameter))
    {
        operatingInBrightFieldMode = true;

        this->gaussianKernel = new GenerateGausssian2ordKernel(framework, particleDiameter, gaussianDiameter);
        this->gaussianKernel->run();

        this->initKernels(framework);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::initKernels(Framework *framework)
    {
        this->sourceConvXX = new ConvolutionOperator(framework, this->gaussianKernel->getResultXX(), this->temporarySource, true);
        this->sourceConvXY = new ConvolutionOperator(framework, this->gaussianKernel->getResultXY(), this->temporarySource, true);
        this->sourceConvYY = new ConvolutionOperator(framework, this->gaussianKernel->getResultYY(), this->temporarySource, true);

        this->eigenSolveKernel = new EigenvaluesOfGaussianKernel(framework, this->sourceConvXX->getResult(), this->sourceConvXY->getResult(), this->sourceConvYY->getResult());
        this->caoKernel = new CaoDetectorKernel(framework, this->eigenSolveKernel->getResultMins(), this->eigenSolveKernel->getResultMaxs());
        this->thresholdKernel = new ThresholdKernel(framework, this->caoKernel->getResult());
        this->nmsKernel = new LocalMaximumSimpleKernel(framework, this->thresholdKernel->getResult(), this->particleSize);
    }

    ConvolutionBasedDetectorWithSecondOrderGaussian::~ConvolutionBasedDetectorWithSecondOrderGaussian()
    {
        delete this->nmsKernel;
        delete this->thresholdKernel;
        delete this->caoKernel;
        delete this->eigenSolveKernel;
        delete this->sourceConvYY;
        delete this->sourceConvXY;
        delete this->sourceConvXX;
        delete this->gaussianKernel;
    }

    float ConvolutionBasedDetectorWithSecondOrderGaussian::getScale()
    {
        return this->gaussianKernel->getSigma();
    }

    Vec2ui ConvolutionBasedDetectorWithSecondOrderGaussian::getParticleSize()
    {
        return this->particleSize;
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::preComputeDirectionalGaussian()
    {
        this->gaussianKernel->run();
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::process(GPUMapped<Image> *source)
    {
        this->temporarySource = source;

        this->sourceConvXX->setInput(source);
        this->sourceConvXY->setInput(source);
        this->sourceConvYY->setInput(source);

        this->process();
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::process()
    {
        this->sourceConvXX->execute();
        this->sourceConvXY->execute();
        this->sourceConvYY->execute();

        this->eigenSolveKernel->run();
        this->caoKernel->run();

        this->adjustThreshold(0.0f);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::adjustThreshold(float value)
    {
        delete this->resultVector;

        this->thresholdKernel->setThreshold(value);
        this->thresholdKernel->run();
        this->nmsKernel->run();

        this->resultImage = this->nmsKernel->getResult();
        this->resultVector = ParticleExtractor::extractVector(this->resultImage);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::lowerResourceProcess(GPUMapped<Image> *convolInXXDir, GPUMapped<Image> *convolInXYDir, GPUMapped<Image> *convolInYYDir)
    {
        this->eigenSolveKernel->setInput(convolInXXDir, convolInXYDir, convolInYYDir);
        this->eigenSolveKernel->run();
        this->caoKernel->run();

        this->adjustThreshold(0.0f);
    }

    bool ConvolutionBasedDetectorWithSecondOrderGaussian::darkFieldSupported()
    {
        return false;
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::doDebugOutput(std::string pathPrefix)
    {
        Logger::getInstance().LogFilename(pathPrefix + "CaoDetector.log");
        Logger::getInstance().activateFileLog(Logger::FORMAT_SIMPLE);

        ImageSerializer::writeImage(pathPrefix + "_1_gau_XX", this->gaussianKernel->getResultXXAsImage(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_1_gau_XY", this->gaussianKernel->getResultXYAsImage(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_1_gau_YY", this->gaussianKernel->getResultYYAsImage(), ImageSerializer::TIFF_GRAY_32);

        ImageSerializer::writeImage(pathPrefix + "_2_conv_XX", this->sourceConvXX->getResultAsImage(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_2_conv_XY", this->sourceConvXY->getResultAsImage(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_2_conv_YY", this->sourceConvYY->getResultAsImage(), ImageSerializer::TIFF_GRAY_32);

        ImageSerializer::writeImage(pathPrefix + "_3_eigen_Min", this->eigenSolveKernel->getResultMins(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_3_eigen_Max", this->eigenSolveKernel->getResultMaxs(), ImageSerializer::TIFF_GRAY_32);

        ImageSerializer::writeImage(pathPrefix + "_4_cao", this->caoKernel->getResult(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_5_thresh0", this->thresholdKernel->getResult(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_6_nms", this->nmsKernel->getResult(), ImageSerializer::TIFF_GRAY_32);

        // Taking zero-thresholded image as input for Decider
        thresholdKernel->getResult()->ensureIsUpToDateOnCPU();
        float entropyThreshold = ThresholdDecider::optimalEntropyThreshold(thresholdKernel->getResult()->getObjectOnCPU());
        thresholdKernel->getResult()->ensureIsUpToDateOnCPU();
        float otsuThreshold = ThresholdDecider::otsuThreshold(thresholdKernel->getResult()->getObjectOnCPU());

        this->adjustThreshold(entropyThreshold);

        std::string particles = "__p";
        ParticleExtractor::saveToFileAll(this->resultVector, pathPrefix + particles + "Entropy" + std::to_string(entropyThreshold) + ".txt", this->particleSize, true);

        if(otsuThreshold != entropyThreshold)
        {
            this->adjustThreshold(otsuThreshold);
            ParticleExtractor::saveToFileAll(this->resultVector, pathPrefix + particles + "Otsu" + std::to_string(otsuThreshold) + ".txt", this->particleSize, true);
        }

        Logger::getInstance().deactivateFileLog();
    }
}