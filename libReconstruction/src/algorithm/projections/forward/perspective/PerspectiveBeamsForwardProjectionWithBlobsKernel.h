#pragma once
#include "algorithm/projections/forward/ForwardProjectionKernel.h"

namespace ettention
{
    class BlobRayIntegrationSampler;
    class CLMemBuffer;

    class PerspectiveBeamsForwardProjectionWithBlobsKernel : public ForwardProjectionKernel
    {
    public:
        PerspectiveBeamsForwardProjectionWithBlobsKernel(Framework* framework, const std::shared_ptr<BlobRayIntegrationSampler>& blobSampler, const GeometricSetup* geometricSetup, GPUMappedVolume* volume);
        ~PerspectiveBeamsForwardProjectionWithBlobsKernel();

    protected:
        void prepareKernelArguments() override;

    private:
        std::shared_ptr<BlobRayIntegrationSampler> blobSampler;
        CLMemBuffer* blobLookupBuffer;
        CLMemBuffer* projection2worldBuffer;
    };
}