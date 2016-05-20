#include "stdafx.h"

#include "gpu/imagerepresentation/NativeGPURepresentationOfImage.h"
                    
namespace ettention
{
    NativeGPURepresentationOfImage::NativeGPURepresentationOfImage(Vec3ui resolution, float *data)
        : GPURepresentationOfImage(resolution), data(data)
    {

    }

    NativeGPURepresentationOfImage::~NativeGPURepresentationOfImage()
    {

    }

    size_t NativeGPURepresentationOfImage::getSizeInBytes()
    {
        return resolution.x * resolution.y * resolution.z * sizeof(float);
    }

    void* NativeGPURepresentationOfImage::getGPURepresentation()
    {
        return (void*)data;
    }

    void NativeGPURepresentationOfImage::convertBackToCPURepresentation(float*)
    {
        return;
    }
}