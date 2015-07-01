#pragma once
#include "model/blobs/BlobParameters.h"

namespace ettention
{
    class BlobRayIntegrationSampler
    {
        friend std::ostream& operator<<(std::ostream& os, const BlobRayIntegrationSampler& sampler);

    public:
        BlobRayIntegrationSampler(const BlobParameters& parameters, unsigned int sampleCount);
        ~BlobRayIntegrationSampler();

        const float* GetSamples() const;
        unsigned int GetSampleCount() const;
        const BlobParameters& GetParameters() const;

    private:
        unsigned int sampleCount;
        float* samples;
        BlobParameters params;

        void GenerateSamples();
        double IntegrateForDistance(double w) const;
        double EvaluateModifiedBessel(double x, unsigned int numberOfSummands) const;

    };

    std::ostream& operator<<(std::ostream& os, const BlobRayIntegrationSampler& sampler);

}