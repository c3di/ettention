#include "stdafx.h"
#include "algorithm/imagestatistics/DeviationOperator.h"
#include "framework/Framework.h"
#include "algorithm/imagestatistics/MeanOperator.h"
#include "algorithm/imagemanipulation/SquareKernel.h"

namespace ettention
{
    DeviationOperator::DeviationOperator(Framework* framework, GPUMapped<Image>* source)
        : abstractionLayer(framework->getOpenCLStack())
    {
        meanOperator = new MeanOperator(framework, source);
        squareKernel = new SquareKernel(framework, source);
        squaredMeanOperator = new MeanOperator(framework, squareKernel->getOutput());
    }

    DeviationOperator::~DeviationOperator()
    {
        delete this->squaredMeanOperator;
        delete this->squareKernel;
        delete this->meanOperator;
    }

    void DeviationOperator::setInput(GPUMapped<Image>* buffer)
    {
        this->meanOperator->setInput(buffer);
        this->squareKernel->setInput(buffer);
    }

    void DeviationOperator::computeDeviation()
    {
        meanOperator->computeMean();
        float computedMean = meanOperator->getMean();
        return this->computeDeviation(computedMean);
    }

    void DeviationOperator::computeDeviation(float mean)
    {
        this->squareKernel->run();
        this->squaredMeanOperator->computeMean();

        float squaredDataMean = this->squaredMeanOperator->getMean();
        this->deviation = sqrt(squaredDataMean - (mean * mean));
    }

    float DeviationOperator::getDeviation()
    {
        return this->deviation;
    }
}