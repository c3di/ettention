#include "stdafx.h"

#include "gpu/imagerepresentation/HalfPrecisionGPURepresentationOfImage.h"
                    
namespace ettention
{
    HalfPrecisionGPURepresentationOfImage::HalfPrecisionGPURepresentationOfImage(Vec3ui resolution, float *data)
        : GPURepresentationOfImage(resolution)
    {
        gpudata.resize(resolution.x * resolution.y * resolution.z);

        for(size_t i = 0; i < gpudata.size(); i++)
            gpudata[i] = (cl_half)data[i];
    }

    HalfPrecisionGPURepresentationOfImage::~HalfPrecisionGPURepresentationOfImage()
    {
    }

    size_t HalfPrecisionGPURepresentationOfImage::getSizeInBytes()
    {
        return resolution.x * resolution.y * resolution.z * sizeof(cl_half);
    }

    void* HalfPrecisionGPURepresentationOfImage::getGPURepresentation()
    {
        return (void*)&(gpudata[0]);
    }

    void HalfPrecisionGPURepresentationOfImage::convertBackToCPURepresentation(float* data)
    {
        for(size_t i = 0; i < gpudata.size(); i++)
            data[i] = (float)gpudata[i];
    }
}