#include "stdafx.h"
#include "PerspectiveBeamsForwardProjectionWithBlobsKernel.h"
#include "PerspectiveBeamsForwardProjectionWithBlobs_bin2c.h"
#include "algorithm/blobs/BlobRayIntegrationSampler.h"
#include "gpu/opencl/CLKernel.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "model/geometry/GeometricSetup.h"

namespace ettention
{
    PerspectiveBeamsForwardProjectionWithBlobsKernel::PerspectiveBeamsForwardProjectionWithBlobsKernel(Framework* framework, const std::shared_ptr<BlobRayIntegrationSampler>& blobSampler, const GeometricSetup* geometricSetup, GPUMappedVolume* volume, GPUMappedVolume* priorKnowledgeMask)
        : ForwardProjectionKernel(framework,
                                  PerspectiveBeamsForwardProjectionWithBlobs_kernel_SourceCode,
                                  "execute",
                                  "PerspectiveBeamsForwardProjectionWithBlobsKernel",
                                  geometricSetup,
                                  volume,
                                  priorKnowledgeMask,
                                  1)
        , blobSampler(blobSampler)
        , blobLookupBuffer(new CLMemBuffer(abstractionLayer, this->blobSampler->getSampleCount() * sizeof(float), blobSampler->getSamples()))
        , projection2worldBuffer(new CLMemBuffer(abstractionLayer, 16 * sizeof(float)))
    {
    }

    PerspectiveBeamsForwardProjectionWithBlobsKernel::~PerspectiveBeamsForwardProjectionWithBlobsKernel()
    {
        delete blobLookupBuffer;
        delete projection2worldBuffer;
    }

    void PerspectiveBeamsForwardProjectionWithBlobsKernel::prepareKernelArguments()
    {
        ForwardProjectionKernel::prepareKernelArguments();
        implementation->setArgument("globalBlobSampler", blobLookupBuffer);
        implementation->allocateArgument("blobSampler", blobSampler->getSampleCount() * sizeof(float));
        implementation->setArgument("blobSampleCount", blobSampler->getSampleCount());
        implementation->setArgument("blobRadius", blobSampler->getParameters().getSupportRadius());
        Matrix4x4 world2projection = getGeometricSetup()->getScannerGeometry()->getProjectionMatrix();
        Matrix4x4 projection2world = world2projection.inverse();
        projection2worldBuffer->transferFrom(projection2world.getData(), 16 * sizeof(float));
        implementation->setArgument("projection2world", projection2worldBuffer);
    }
}