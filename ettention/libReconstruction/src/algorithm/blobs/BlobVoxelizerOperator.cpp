#include "stdafx.h"
#include "BlobVoxelizerOperator.h"
#include "algorithm/blobs/BlobVoxelizationKernelValues.h"
#include "algorithm/volumemanipulation/VolumeConvolutionOperator.h"
#include "framework/Framework.h"
#include "model/volume/FloatVolume.h"

namespace ettention
{
    BlobVoxelizerOperator::BlobVoxelizerOperator(Framework* framework, GPUMappedVolume* blobVolume, const BlobParameters& blobParameters)
        : blobParameters(blobParameters)
    {
        BlobVoxelizationKernelValues values(blobParameters);
        kernelValues = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(values.getKernelResolution(), values.getKernelValues()));
        voxelVolume = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(blobVolume->getObjectOnCPU()->getProperties().getVolumeResolution(), 0.0f));
        convolutionOperator = new VolumeConvolutionOperator(framework, blobVolume, voxelVolume, kernelValues);
    }

    BlobVoxelizerOperator::~BlobVoxelizerOperator()
    {
        delete convolutionOperator;
        delete voxelVolume->getObjectOnCPU();
        delete voxelVolume;
        delete kernelValues->getObjectOnCPU();
        delete kernelValues;
    }

    GPUMappedVolume* BlobVoxelizerOperator::getVoxelVolume() const
    {
        return voxelVolume;
    }

    void BlobVoxelizerOperator::voxelize()
    {
        convolutionOperator->run();
    }
}