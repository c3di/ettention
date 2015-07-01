#include "stdafx.h"
#include "CrosscorrelationOperator.h"
#include "framework/Framework.h"
#include "model/image/Image.h"
#include "algorithm/imagestatistics/MeanOperator.h"

namespace ettention
{
    CrosscorrelationOperator::CrosscorrelationOperator(Framework *framework, GPUMapped<Image> *reference, GPUMapped<Image> *candidate)
        : framework(framework)
    {
        nccCKernel = new CrosscorrelationClampingKernel(framework, reference, candidate);
        nccWKernel = new CrosscorrelationWarpingKernel(framework, reference, candidate);

        currentKernel = nccCKernel;
        maxKernel = new GlobalMaximumKernel(framework, currentKernel->getResult());

        prepareData(reference, candidate); // Compute Means and Deviations
    }

    CrosscorrelationOperator::~CrosscorrelationOperator()
    {
        delete this->nccCKernel;
        delete this->nccWKernel;
        delete this->maxKernel;
    }

    void CrosscorrelationOperator::setInput(GPUMapped<Image>* reference, GPUMapped<Image>* candidate)
    {
        this->nccCKernel->setInputs(reference, candidate);
        this->nccWKernel->setInputs(reference, candidate);

        prepareData(reference, candidate);
    }

    void CrosscorrelationOperator::prepareData(GPUMapped<Image>* reference, GPUMapped<Image>* candidate)
    {
        MeanOperator* referenceMeanOperator = new MeanOperator(framework, reference);
        MeanOperator* candidateMeanOperator = new MeanOperator(framework, candidate);

        referenceMeanOperator->computeMean();
        candidateMeanOperator->computeMean();

        float referenceMean = referenceMeanOperator->getMean();
        float candidateMean = candidateMeanOperator->getMean();
        this->means = Vec2f(referenceMean, candidateMean);

        DeviationOperator* referenceDeviationOperator = new DeviationOperator(framework, reference);
        DeviationOperator* candidateDeviationOperator = new DeviationOperator(framework, candidate);

        referenceDeviationOperator->computeDeviation(referenceMean);
        candidateDeviationOperator->computeDeviation(candidateMean);

        float referenceDeviation = referenceDeviationOperator->getDeviation();
        float candidateDeviation = candidateDeviationOperator->getDeviation();
        this->deviations = Vec2f(referenceDeviation, candidateDeviation);

        delete candidateDeviationOperator;
        delete referenceDeviationOperator;

        delete candidateMeanOperator;
        delete referenceMeanOperator;
    }

    void CrosscorrelationOperator::matchTemplate()
    {
        this->currentKernel = this->nccCKernel;
        this->computeCorrelation();
    }

    void CrosscorrelationOperator::findTranslation()
    {
        this->currentKernel = this->nccWKernel;
        this->computeCorrelation();
    }

    void CrosscorrelationOperator::computeCorrelation()
    {
        this->currentKernel->setCandidateMean(this->means.y);
        this->currentKernel->run();

        this->maxKernel->setInput(this->currentKernel->getResult());
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
        return this->maxKernel->getMaximumPosition();
    }

    float CrosscorrelationOperator::getPeakValue()
    {
        return this->maxKernel->getMaximum();
    }

    GPUMapped<Image> *CrosscorrelationOperator::getResult()
    {
        return this->currentKernel->getResult();
    }

    Image *CrosscorrelationOperator::getResultAsImage()
    {
        this->currentKernel->getResult()->ensureIsUpToDateOnCPU();
        return this->currentKernel->getResult()->getObjectOnCPU();
    }
}