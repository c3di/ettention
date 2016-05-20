#include "stdafx.h"
#include "BlobRayIntegrationSampler.h"
#include "framework/NumericalAlgorithms.h"

namespace ettention
{
    BlobRayIntegrationSampler::BlobRayIntegrationSampler(const BlobParameters& parameters, unsigned int sampleCount)
        : params(parameters)
        , sampleCount(sampleCount)
        , samples(new float[sampleCount])
    {
        generateSamples();
    }

    BlobRayIntegrationSampler::~BlobRayIntegrationSampler()
    {
        delete[] samples;
    }

    const float* BlobRayIntegrationSampler::getSamples() const
    {
        return samples;
    }

    unsigned int BlobRayIntegrationSampler::getSampleCount() const
    {
        return sampleCount;
    }

    const BlobParameters& BlobRayIntegrationSampler::getParameters() const
    {
        return params;
    }

    void BlobRayIntegrationSampler::generateSamples()
    {
        double a = params.getSupportRadius();
        for(unsigned int i = 0; i < sampleCount; ++i)
        {
            double w = (double)i * a / (double)(sampleCount - 1);
            samples[i] = (float)integrateForDistance(w);
        }
    }

#define NUMBER_OF_INTEGRAL_SUMMANDS 1000
    double BlobRayIntegrationSampler::integrateForDistance(double w) const
    {
        unsigned int n = NUMBER_OF_INTEGRAL_SUMMANDS;
        double a = params.getSupportRadius();
        double d = sqrt(a * a - w * w);
        double delta = d / (double)n;
        double sum = 0.0;
        for(unsigned int k = 1; k <= n; ++k)
        {
            double w_k = sqrt(w * w + (double)(k * k) * delta * delta);
            sum += params.evaluate(w_k);
        }
        return delta * (2.0 * sum + params.evaluate(w));
    }

    std::ostream& operator<<(std::ostream& os, const BlobRayIntegrationSampler& sampler)
    {
        os << "BlobSampler: a=" << sampler.params.getSupportRadius() << ", m=" << sampler.params.getDegree() << ", alpha=" << sampler.params.getDensityFallOff() << "\nValues: ";
        for(unsigned int i = 0; i < sampler.sampleCount; ++i)
        {
            os << sampler.samples[i] << "\n";
        }
        return os;
    }

}