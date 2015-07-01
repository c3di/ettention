#include "stdafx.h"
#include "BlobVoxelizerSampler.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    BlobVoxelizerSampler::BlobVoxelizerSampler(const BlobParameters& params)
        : parameters(params)
        , radius((int)ceil(params.GetSupportRadius()))
    {
        this->GenerateSamples();
    }

    BlobVoxelizerSampler::~BlobVoxelizerSampler()
    {
        
    }

    unsigned int BlobVoxelizerSampler::GetSampleCount() const
    {
        return (unsigned int)samples.size();
    }

    const float* BlobVoxelizerSampler::GetSamples() const
    {
        return &samples[0];
    }

    const BlobParameters& BlobVoxelizerSampler::GetParameters() const
    {
        return parameters;
    }

    void BlobVoxelizerSampler::GenerateSamples()
    {
        samples.reserve((2 * radius + 1) * (2 * radius + 1) * (2 * radius + 1));
        for(int dz = -radius; dz <= radius; ++dz)
        {
            for(int dy = -radius; dy <= radius; ++dy)
            {
                for(int dx = -radius; dx <= radius; ++dx)
                {
                    float val = (float)this->IntegrateBlobOverVoxel(Vec3i(dx, dy, dz));
                    samples.push_back(val);
                }
            }
        }
    }

#define SAMPLES 20
    double BlobVoxelizerSampler::IntegrateBlobOverVoxel(const Vec3i& voxel) const
    {
        double subVoxelVolume = 1.0 / (double)(SAMPLES * SAMPLES * SAMPLES);
        double val = 0.0;
        for(int sz = 0; sz < SAMPLES; ++sz)
        {
            for(int sy = 0; sy < SAMPLES; ++sy)
            {
                for(int sx = 0; sx < SAMPLES; ++sx)
                {
                    Vec3d samplePos((double)voxel.x - 0.5f + ((double)sx + 0.5) / (double)SAMPLES,
                                    (double)voxel.y - 0.5f + ((double)sy + 0.5) / (double)SAMPLES,
                                    (double)voxel.z - 0.5f + ((double)sz + 0.5) / (double)SAMPLES);
                    val += subVoxelVolume * parameters.Evaluate(samplePos.Length());
                }
            }
        }
        return val;
    }

}