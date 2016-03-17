#include "stdafx.h"
#include "CrosscorrelationOperator.h"
#include "framework/Framework.h"
#include "model/image/Image.h"
#include "algorithm/imagestatistics/MeanOperator.h"
#include "algorithm/imagestatistics/DeviationOperator.h"

namespace ettention
{
    CrosscorrelationOperator::CrosscorrelationOperator(Framework *framework, GPUMapped<Image> *reference, GPUMapped<Image> *candidate)
        : framework(framework)
    {
        nccClampingKernel   = new CrosscorrelationClampingKernel(framework, reference, candidate);
        nccWarpingKernel    = new CrosscorrelationWarpingKernel (framework, reference, candidate);

        currentKernel = nccClampingKernel;
        maxKernel = new GlobalMaximumPositionImageKernel(framework, currentKernel->getOutput());

        precomputeMeansAndDeviations(reference, candidate);
    }

    CrosscorrelationOperator::~CrosscorrelationOperator()
    {
        delete nccClampingKernel;
        delete nccWarpingKernel;
        delete maxKernel;
    }

    void CrosscorrelationOperator::setInput(GPUMapped<Image>* reference, GPUMapped<Image>* candidate)
    {
        nccClampingKernel->setInputs(reference, candidate);
        nccWarpingKernel->setInputs(reference, candidate);
        precomputeMeansAndDeviations(reference, candidate);
    }

    void CrosscorrelationOperator::precomputeMeansAndDeviations(GPUMapped<Image>* reference, GPUMapped<Image>* candidate)
    {
        precomputeMeans(reference, candidate);
        precomputeDeviations(reference, candidate);
    }

    void CrosscorrelationOperator::precomputeMeans(GPUMapped<Image>* reference, GPUMapped<Image>* candidate)
    {
        MeanOperator* referenceMeanOperator = new MeanOperator(framework, reference);
        MeanOperator* candidateMeanOperator = new MeanOperator(framework, candidate);

        referenceMeanOperator->computeMean();
        candidateMeanOperator->computeMean();

        float referenceMean = referenceMeanOperator->getMean();
        float candidateMean = candidateMeanOperator->getMean();

        this->means = Vec2f(referenceMean, candidateMean);

        delete candidateMeanOperator;
        delete referenceMeanOperator;
    }

    void CrosscorrelationOperator::precomputeDeviations(GPUMapped<Image>* reference, GPUMapped<Image>* candidate)
    {
        DeviationOperator* referenceDeviationOperator = new DeviationOperator(framework, reference);
        DeviationOperator* candidateDeviationOperator = new DeviationOperator(framework, candidate);

        referenceDeviationOperator->computeDeviation(this->means.x);
        candidateDeviationOperator->computeDeviation(this->means.y);

        float referenceDeviation = referenceDeviationOperator->getDeviation();
        float candidateDeviation = candidateDeviationOperator->getDeviation();

        this->deviations = Vec2f(referenceDeviation, candidateDeviation);

        delete candidateDeviationOperator;
        delete referenceDeviationOperator;
    }

    void CrosscorrelationOperator::matchTemplate()
    {
        this->currentKernel = this->nccClampingKernel;
        this->computeCorrelation();
    }

    void CrosscorrelationOperator::findTranslation()
    {
        this->currentKernel = this->nccWarpingKernel;
        this->computeCorrelation();
    }

    void CrosscorrelationOperator::computeCorrelation()
    {
        this->currentKernel->setCandidateMean(this->means.y);
        this->currentKernel->run();

        this->maxKernel->setInputImage(this->currentKernel->getOutput());
        this->maxKernel->run();
    }

    Vec2f CrosscorrelationOperator::getReferenceImageInfo()
    {
        return Vec2f(this->means.x, this->deviations.x);
    }

    Vec2f CrosscorrelationOperator::getCandidateImageInfo()
    {
        return Vec2f(this->means.y, this->deviations.y);
    }

    Vec2ui CrosscorrelationOperator::getPeakPosition()
    {
        return this->maxKernel->getMaximumXY();
    }

    float CrosscorrelationOperator::getPeakValue()
    {
        return this->maxKernel->getMaximumValue();
    }

    GPUMapped<Image> *CrosscorrelationOperator::getOutput() const
    {
        return this->currentKernel->getOutput();
    }
}