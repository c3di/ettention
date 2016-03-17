#include "stdafx.h"
#include "algorithm/imagemanipulation/MissingWedgeImageGenerator.h"
#include "algorithm/imagemanipulation/fft/PhaseRandomizationOperator.h"
#include "algorithm/imagestatistics/histogram/HistogramEqualization.h"
#include "gpu/GPUMapped.h"
#include "model/image/Image.h"

namespace ettention
{
    MissingWedgeImageGenerator::MissingWedgeImageGenerator(Framework* framework, GPUMapped<Image>* inputImage)
        : abstractionLayer{ abstractionLayer }
        , inputImage{ inputImage }
    {
        phaseRandomizationOperator = new PhaseRandomizationOperator(framework, inputImage);
        randomImage = 0;
    }

    MissingWedgeImageGenerator::~MissingWedgeImageGenerator()
    {
        delete phaseRandomizationOperator;
        delete randomImage;
    }

    Image* MissingWedgeImageGenerator::getOutput()
    {
        return randomImage;
    }

    void MissingWedgeImageGenerator::run()
    {
        phaseRandomizationOperator->setRandomPhase();
        phaseRandomizationOperator->run();
        randomImage = HistogramEqualization::equalizeHistograms(inputImage->getObjectOnCPU(), phaseRandomizationOperator->getOutput());
    }
}