#pragma once
#include "framework/math/Vec3.h"
#include "model/blobs/BlobParameters.h"

namespace ettention
{
    class BlobVoxelizationKernelValues
    {
    public:
        BlobVoxelizationKernelValues(const BlobParameters& blobParameters);
        ~BlobVoxelizationKernelValues();

        const float* getKernelValues() const;
        Vec3ui getKernelResolution() const;

    private:
        BlobParameters blobParameters;
        std::vector<float> values;

        void generateValues();
        double integrateBlobOverVoxel(const Vec3i& voxel) const;
        unsigned int getValuesPerDimension(void) const;
    };
}