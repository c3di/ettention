#include "stdafx.h"
#include "algorithm/imagemanipulation/fft/PhaseRandomizationOperator.h"
#include "algorithm/imagemanipulation/fft/PhaseRandomizationKernel.h"
#include "algorithm/imagemanipulation/fft/FFTForwardKernel.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"
#include "algorithm/imagemanipulation/fft/FFTPhaseKernel.h"
#include "algorithm/imagemanipulation/fft/FFTMagnitudeKernel.h"
#include "framework/Framework.h"
#include "framework/RandomAlgorithms.h"


namespace ettention
{
    PhaseRandomizationOperator::PhaseRandomizationOperator(Framework* framework, GPUMapped<Image>* imageInput)
        : abstractionLayer(framework->getOpenCLStack())
        , imageInput(imageInput)
    {
        fftForwardKernel = new FFTForwardKernel(framework->getOpenCLStack(), imageInput);
        fftMagnitudeKernel = new FFTMagnitudeKernel(framework, fftForwardKernel->getOutputRealPart(), fftForwardKernel->getOutputImaginaryPart());

        Image* initRandomImage = new Image(imageInput->getResolution());

        randomImage = new GPUMapped<Image>(abstractionLayer,initRandomImage);
        randomImage->takeOwnershipOfObjectOnCPU();

        phaseRandomizationKernel = new PhaseRandomizationKernel(framework, fftMagnitudeKernel->getOutput(), randomImage);

        fftBackKernel = new FFTBackKernel(abstractionLayer, phaseRandomizationKernel->getRealOutput(), phaseRandomizationKernel->getImaginaryOutput());
    }

    PhaseRandomizationOperator::~PhaseRandomizationOperator()
    {
        delete fftForwardKernel;
        delete fftMagnitudeKernel;
        delete randomImage;
        delete phaseRandomizationKernel;
        delete fftBackKernel;
    }

    void PhaseRandomizationOperator::run()
    {
        fftForwardKernel->run();
        fftMagnitudeKernel->run();
        phaseRandomizationKernel->run();
        fftBackKernel->run();
    }

    void PhaseRandomizationOperator::setRandomPhase(unsigned int seed)
    {
        Image* randomImageCPU = RandomAlgorithms::generateRandomImage(imageInput->getResolution().x, imageInput->getResolution().y, seed);
        randomImage->setObjectOnCPU(randomImageCPU);
        randomImage->takeOwnershipOfObjectOnCPU();
        randomImage->ensureIsUpToDateOnGPU();
    }

    void PhaseRandomizationOperator::setInputImage(GPUMapped<Image>* inputImage)
    {
        imageInput = inputImage;
    }

    Image* PhaseRandomizationOperator::getOutput()
    {
        fftBackKernel->getOutput()->ensureIsUpToDateOnCPU();
        return fftBackKernel->getOutput()->getObjectOnCPU();
    }

}