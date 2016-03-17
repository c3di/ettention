#include "stdafx.h"
#include "BlobVoxelizationKernelValues.h"

namespace ettention
{
    BlobVoxelizationKernelValues::BlobVoxelizationKernelValues(const BlobParameters& blobParameters)
        : blobParameters(blobParameters)
    {
        generateValues();
    }

    BlobVoxelizationKernelValues::~BlobVoxelizationKernelValues()
    {
    }

    Vec3ui BlobVoxelizationKernelValues::getKernelResolution() const
    {
        return getValuesPerDimension() * Vec3ui(1, 1, 1);
    }

    const float* BlobVoxelizationKernelValues::getKernelValues() const
    {
        return &values[0];
    }

    unsigned int BlobVoxelizationKernelValues::getValuesPerDimension() const
    {
        return 2U * (unsigned int)std::ceil(blobParameters.getSupportRadius()) + 1U;
    }

    void BlobVoxelizationKernelValues::generateValues()
    {
        float sum = 0.0f;
        int radius = (int)std::ceil(blobParameters.getSupportRadius());
        for(int dz = -radius; dz <= radius; ++dz)
        {
            for(int dy = -radius; dy <= radius; ++dy)
            {
                for(int dx = -radius; dx <= radius; ++dx)
                {
                    float val = (float)this->integrateBlobOverVoxel(Vec3i(dx, dy, dz));
                    sum += val;
                    values.push_back(val);
                }
            }
        }
        std::cout << "Blob integral sum: " << sum << std::endl;
    }

#define SAMPLES 20
    double BlobVoxelizationKernelValues::integrateBlobOverVoxel(const Vec3i& voxel) const
    {
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
                    val += blobParameters.evaluate(samplePos.getLength());
                }
            }
        }
        return val / (double)(SAMPLES * SAMPLES * SAMPLES);
    }
}