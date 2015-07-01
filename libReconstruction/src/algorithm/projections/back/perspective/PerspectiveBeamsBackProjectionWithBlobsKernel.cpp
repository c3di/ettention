#include "stdafx.h"
#include "PerspectiveBeamsBackProjectionWithBlobsKernel.h"
#include "PerspectiveBeamsBackProjectionWithBlobs_bin2c.h"
#include "algorithm/blobs/BlobRayIntegrationSampler.h"
#include "gpu/opencl/CLKernel.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "model/geometry/GeometricSetup.h"

namespace ettention
{
    PerspectiveBeamsBackProjectionWithBlobsKernel::PerspectiveBeamsBackProjectionWithBlobsKernel(Framework* framework, 
                                                                                                 const GeometricSetup* geometricSetup, 
                                                                                                 GPUMappedVolume* volume,
                                                                                                 const BlobParameters& blobParams,
                                                                                                 float lambda, 
                                                                                                 bool useLongObjectCompensation)
        : BackProjectionKernel(framework,
                               PerspectiveBeamsBackProjectionWithBlobs_kernel_SourceCode,
                               "execute",
                               "PerspectiveBeamsBackProjectionWithBlobsKernel",
                               geometricSetup,
                               volume,
                               0,
                               lambda, 
                               1,
                               useLongObjectCompensation,
                               true)
        , blobSampler(new BlobRayIntegrationSampler(blobParams, 256))
        , world2projectionBuffer(new CLMemBuffer(abstractionLayer, 16 * sizeof(float)))
        , projection2worldBuffer(new CLMemBuffer(abstractionLayer, 16 * sizeof(float)))
    {
        blobLookupBuffer = new CLMemBuffer(abstractionLayer, blobSampler->GetSampleCount() * sizeof(float), blobSampler->GetSamples());
        if(useLongObjectCompensation)
        {
            throw Exception("Long object compensation with blobs not yet implemented!");
        }
    }

    PerspectiveBeamsBackProjectionWithBlobsKernel::~PerspectiveBeamsBackProjectionWithBlobsKernel()
    {
        delete blobLookupBuffer;
        delete projection2worldBuffer;
        delete world2projectionBuffer;
        delete blobSampler;
    }

    void PerspectiveBeamsBackProjectionWithBlobsKernel::prepareKernelArguments()
    {
        BackProjectionKernel::prepareKernelArguments();
        implementation->setArgument("blobSampler", blobLookupBuffer);
        implementation->setArgument("blobSampleCount", blobSampler->GetSampleCount());
        implementation->setArgument("blobRadius", blobSampler->GetParameters().GetSupportRadius());
        Matrix4x4 world2projection = geometricSetup->getScannerGeometry().getProjectionMatrix();
        Matrix4x4 projection2world = world2projection.inverse();
        world2projectionBuffer->transferFrom(world2projection.getData());
        projection2worldBuffer->transferFrom(projection2world.getData());
        implementation->setArgument("world2projection", world2projectionBuffer);
        implementation->setArgument("projection2world", projection2worldBuffer);
    }
}