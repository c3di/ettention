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
        : ConvolutionBasedDetector(framework, Vec2ui(particleDiameter, particleDiameter), false, true)
        , resolution(source->getResolution())
        , source(source)
    {
        this->gaussianKernel = new GenerateGausssian2ordKernel(framework, particleDiameter);
        init(framework);
    }

    ConvolutionBasedDetectorWithSecondOrderGaussian::ConvolutionBasedDetectorWithSecondOrderGaussian(Framework *framework, GPUMapped<Image> *source, unsigned int particleDiameter, unsigned int gaussianDiameter)
        : ConvolutionBasedDetector(framework, Vec2ui(particleDiameter, particleDiameter), false, true)
        , resolution(source->getResolution())
        , source(source)
    {
        this->gaussianKernel = new GenerateGausssian2ordKernel(framework, particleDiameter, gaussianDiameter);
        init(framework);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::init(Framework *framework)
    {
        isOperatingInBrightFieldMode = true;

        this->sourceConvXX = new GPUMapped<Image>(framework->getOpenCLStack(), resolution);
        this->sourceConvXY = new GPUMapped<Image>(framework->getOpenCLStack(), resolution);
        this->sourceConvYY = new GPUMapped<Image>(framework->getOpenCLStack(), resolution);

        this->convolutionOperator = new ConvolutionOperator(framework, gaussianKernel->getResultXX(), source, sourceConvXX, true);

        this->preComputeDirectionalGaussian();
        this->initRemainingKernels(framework);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::preComputeDirectionalGaussian()
    {
        this->gaussianKernel->run();
        this->preComputeConvolutionWithInput();
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::preComputeConvolutionWithInput()
    {
        this->convolutionOperator->setInput(source);

        this->convolutionOperator->setKernel(this->gaussianKernel->getResultXX(), true);
        this->convolutionOperator->setOutput(this->sourceConvXX);
        this->convolutionOperator->execute();

        this->convolutionOperator->setKernel(this->gaussianKernel->getResultXY(), true);
        this->convolutionOperator->setOutput(this->sourceConvXY);
        this->convolutionOperator->execute();

        this->convolutionOperator->setKernel(this->gaussianKernel->getResultYY(), true);
        this->convolutionOperator->setOutput(this->sourceConvYY);
        this->convolutionOperator->execute();
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::initRemainingKernels(Framework *framework)
    {
        this->eigenSolveKernel  = new EigenvaluesOfGaussianKernel(framework, sourceConvXX, sourceConvXY, sourceConvYY);
        this->caoKernel         = new CaoDetectorKernel(framework, this->eigenSolveKernel->getResultMins(), this->eigenSolveKernel->getResultMaxs());
        this->thresholdKernel   = new ThresholdKernel(framework, this->caoKernel->getOutput());
        this->nmsKernel         = new LocalMaximumSimpleKernel(framework, this->thresholdKernel->getOutput(), this->particleSize);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::clearPreComputedConvolution()
    {
        delete this->sourceConvYY;
        delete this->sourceConvXY;
        delete this->sourceConvXX;
    }

    ConvolutionBasedDetectorWithSecondOrderGaussian::~ConvolutionBasedDetectorWithSecondOrderGaussian()
    {
        clearPreComputedConvolution();

        delete this->nmsKernel;
        delete this->thresholdKernel;
        delete this->caoKernel;
        delete this->eigenSolveKernel;
        delete this->convolutionOperator;
        delete this->gaussianKernel;
    }

    float ConvolutionBasedDetectorWithSecondOrderGaussian::getScale()
    {
        return this->gaussianKernel->getSigma();
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::process(GPUMapped<Image> *source)
    {
        this->source = source;
        this->preComputeConvolutionWithInput();

        this->eigenSolveKernel->run();
        this->caoKernel->run();
        this->process();
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::process()
    {
        this->adjustThreshold(0.0f);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::adjustThreshold(float value)
    {
        delete this->resultVector;

        this->thresholdKernel->setThreshold(value);
        this->thresholdKernel->run();
        this->nmsKernel->run();

        this->resultImage = this->nmsKernel->getOutput();
        this->resultVector = ParticleExtractor::exportParticlesFromImageToListAll(this->resultImage);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::lowerResourceProcess(GPUMapped<Image> *convolInXXDir, GPUMapped<Image> *convolInXYDir, GPUMapped<Image> *convolInYYDir)
    {
        this->eigenSolveKernel->setInput(convolInXXDir, convolInXYDir, convolInYYDir);
        this->eigenSolveKernel->run();
        this->caoKernel->run();

        this->adjustThreshold(0.0f);
    }

    void ConvolutionBasedDetectorWithSecondOrderGaussian::doDebugOutput(std::string pathPrefix)
    {
        Logger::getInstance().setLogFilename(pathPrefix + "CaoDetector.log");
        Logger::getInstance().activateFileLog(Logger::FORMAT_SIMPLE);

        ImageSerializer::writeImage(pathPrefix + "_1_gau_XX", this->gaussianKernel->getResultXXAsImage(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_1_gau_XY", this->gaussianKernel->getResultXYAsImage(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_1_gau_YY", this->gaussianKernel->getResultYYAsImage(), ImageSerializer::TIFF_GRAY_32);

        ImageSerializer::writeImage(pathPrefix + "_2_conv_XX", this->sourceConvXX->getObjectOnCPU(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_2_conv_XY", this->sourceConvXY->getObjectOnCPU(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_2_conv_YY", this->sourceConvYY->getObjectOnCPU(), ImageSerializer::TIFF_GRAY_32);

        ImageSerializer::writeImage(pathPrefix + "_3_eigen_Min", this->eigenSolveKernel->getResultMins(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_3_eigen_Max", this->eigenSolveKernel->getResultMaxs(), ImageSerializer::TIFF_GRAY_32);

        ImageSerializer::writeImage(pathPrefix + "_4_cao",      this->caoKernel->getOutput(),       ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_5_thresh0",  this->thresholdKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
        ImageSerializer::writeImage(pathPrefix + "_6_nms",      this->nmsKernel->getOutput(),       ImageSerializer::TIFF_GRAY_32);

        // Taking zero-thresholded image as input for Decider
        thresholdKernel->getOutput()->ensureIsUpToDateOnCPU();
        float entropyThreshold = ThresholdDecider::optimalEntropyThreshold(thresholdKernel->getOutput()->getObjectOnCPU());
        thresholdKernel->getOutput()->ensureIsUpToDateOnCPU();
        float otsuThreshold = ThresholdDecider::otsuThreshold(thresholdKernel->getOutput()->getObjectOnCPU());

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