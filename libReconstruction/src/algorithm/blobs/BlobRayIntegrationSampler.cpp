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
        this->GenerateSamples();
    }

    BlobRayIntegrationSampler::~BlobRayIntegrationSampler()
    {
        delete[] samples;
    }

    const float* BlobRayIntegrationSampler::GetSamples() const
    {
        return samples;
    }

    unsigned int BlobRayIntegrationSampler::GetSampleCount() const
    {
        return sampleCount;
    }

    const BlobParameters& BlobRayIntegrationSampler::GetParameters() const
    {
        return params;
    }

    void BlobRayIntegrationSampler::GenerateSamples()
    {
        double a = params.GetSupportRadius();
        for(unsigned int i = 0; i < sampleCount; ++i)
        {
            double w = (double)i * a / (double)(sampleCount - 1);
            samples[i] = (float)this->IntegrateForDistance(w);
        }
    }

#define NUMBER_OF_INTEGRAL_SUMMANDS 1000
    double BlobRayIntegrationSampler::IntegrateForDistance(double w) const
    {
        unsigned int n = NUMBER_OF_INTEGRAL_SUMMANDS;
        double a = params.GetSupportRadius();
        double d = sqrt(a * a - w * w);
        double delta = d / (double)n;
        double sum = 0.0;
        for(unsigned int k = 1; k <= n; ++k)
        {
            double w_k = sqrt(w * w + (double)(k * k) * delta * delta);
            sum += params.Evaluate(w_k);
        }
        return delta * (2.0 * sum + params.Evaluate(w));
    }

    std::ostream& operator<<(std::ostream& os, const BlobRayIntegrationSampler& sampler)
    {
        os << "BlobSampler: a=" << sampler.params.GetSupportRadius() << ", m=" << sampler.params.GetDegree() << ", alpha=" << sampler.params.GetDensityFallOff() << "\nValues: ";
        for(unsigned int i = 0; i < sampler.sampleCount; ++i)
        {
            os << sampler.samples[i] << "\n";
        }
        return os;
    }

}