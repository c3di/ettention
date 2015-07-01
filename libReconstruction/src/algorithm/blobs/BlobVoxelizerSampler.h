#pragma once
#include "framework/math/Vec3.h"
#include "model/blobs/BlobParameters.h"

namespace ettention
{
    class BlobVoxelizerSampler
    {
    private:
        BlobParameters parameters;
        std::vector<float> samples;
        int radius;

        void GenerateSamples();
        double IntegrateBlobOverVoxel(const Vec3i& voxel) const;

    public:
        BlobVoxelizerSampler(const BlobParameters& params);
        ~BlobVoxelizerSampler();

        const float* GetSamples() const;
        unsigned int GetSampleCount() const;
        const BlobParameters& GetParameters() const;
    };
}