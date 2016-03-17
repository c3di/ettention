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

        const float* getSamples() const;
        unsigned int getSampleCount() const;
        const BlobParameters& getParameters() const;

    private:
        unsigned int sampleCount;
        float* samples;
        BlobParameters params;

        void generateSamples();
        double integrateForDistance(double w) const;
        double evaluateModifiedBessel(double x, unsigned int numberOfSummands) const;

    };

    std::ostream& operator<<(std::ostream& os, const BlobRayIntegrationSampler& sampler);
}