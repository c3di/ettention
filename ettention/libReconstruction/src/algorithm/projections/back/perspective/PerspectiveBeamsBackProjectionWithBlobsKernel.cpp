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
                                                                                                 GPUMappedVolume* priorKnowledgeMask,
                                                                                                 const BlobParameters& blobParams,
                                                                                                 float lambda, 
                                                                                                 bool useLongObjectCompensation)
        : BackProjectionKernel(framework,
                               PerspectiveBeamsBackProjectionWithBlobs_kernel_SourceCode,
                               "correct",
                               "Backprojection (perspective,basis:blobs)",
                               geometricSetup,
                               volume,
                               priorKnowledgeMask,
                               lambda, 
                               1,
                               useLongObjectCompensation,
                               true)
        , blobSampler(new BlobRayIntegrationSampler(blobParams, 256))
        , world2projectionBuffer(new CLMemBuffer(abstractionLayer, 16 * sizeof(float)))
        , projection2worldBuffer(new CLMemBuffer(abstractionLayer, 16 * sizeof(float)))
    {
        blobLookupBuffer = new CLMemBuffer(abstractionLayer, blobSampler->getSampleCount() * sizeof(float), blobSampler->getSamples());
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
        implementation->setArgument("blobSampleCount", blobSampler->getSampleCount());
        implementation->setArgument("blobRadius", blobSampler->getParameters().getSupportRadius());
        Matrix4x4 world2projection = geometricSetup->getScannerGeometry()->getProjectionMatrix();
        Matrix4x4 projection2world = world2projection.inverse();
        world2projectionBuffer->transferFrom(world2projection.getData(), 16 * sizeof(float));
        projection2worldBuffer->transferFrom(projection2world.getData(), 16 * sizeof(float));
        implementation->setArgument("world2projection", world2projectionBuffer);
        implementation->setArgument("projection2world", projection2worldBuffer);
    }
}