#pragma once
#include "algorithm/projections/back/BackProjectionKernel.h"
#include "model/blobs/BlobParameters.h"

namespace ettention
{
    class BlobRayIntegrationSampler;
    class CLMemBuffer;

    class PerspectiveBeamsBackProjectionWithBlobsKernel : public BackProjectionKernel
    {
    private:
        BlobRayIntegrationSampler* blobSampler;
        CLMemBuffer* blobLookupBuffer;
        CLMemBuffer* world2projectionBuffer;
        CLMemBuffer* projection2worldBuffer;

    public:
        PerspectiveBeamsBackProjectionWithBlobsKernel(Framework* framework,
                                                      const GeometricSetup* geometricSetup,
                                                      GPUMappedVolume* volume,
                                                      const BlobParameters& blobParams,
                                                      float lambda, 
                                                      bool useLongObjectCompensation);
        ~PerspectiveBeamsBackProjectionWithBlobsKernel();

    protected:
        void prepareKernelArguments() override;
    };
}