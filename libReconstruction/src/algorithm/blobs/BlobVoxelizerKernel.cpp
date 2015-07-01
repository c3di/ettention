#include "stdafx.h"
#include "BlobVoxelizerKernel.h"
#include "BlobVoxelizer_bin2c.h"
#include "framework/Framework.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    BlobVoxelizerKernel::BlobVoxelizerKernel(Framework* framework, const BlobParameters& blobParameters, GPUMappedVolume* blobVolume, GPUMappedVolume* voxelVolume)
        : ComputeKernel(framework, BlobVoxelizer_kernel_SourceCode, "voxelize", "BlobVoxelizerKernel")
        , sampler(blobParameters)
        , blobVolume(blobVolume)
        , voxelVolume(voxelVolume)
    {
        if(blobVolume->getMappedSubVolume()->getResolution() != voxelVolume->getMappedSubVolume()->getResolution())
        {
            throw Exception("Subvolumes for BlobVoxelizer differ in resolution!");
        }
        samplerBuffer = new CLMemBuffer(framework->getOpenCLStack(), sampler.GetSampleCount() * sizeof(float), sampler.GetSamples());
    }

    BlobVoxelizerKernel::~BlobVoxelizerKernel()
    {
        delete samplerBuffer;
    }

    void BlobVoxelizerKernel::prepareKernelArguments()
    {
        implementation->setArgument("blobVolume", blobVolume->getMappedSubVolume());
        implementation->setArgument("voxelVolume", voxelVolume->getMappedSubVolume());
        implementation->setArgument("blobVoxelizerSampler", samplerBuffer);
        implementation->setArgument("blobVolumeRes", blobVolume->getMappedSubVolume()->getResolution());
        implementation->setArgument("blobRadius", (int)ceil(sampler.GetParameters().GetSupportRadius()));
    }

    void BlobVoxelizerKernel::preRun()
    {
        implementation->setGlobalWorkSize(voxelVolume->getMappedSubVolume()->getResolution());
    }

    void BlobVoxelizerKernel::postRun()
    {
        voxelVolume->markAsChangedOnGPU();
    }
}