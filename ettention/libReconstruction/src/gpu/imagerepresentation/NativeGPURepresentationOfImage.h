#pragma once

#include "gpu/imagerepresentation/GPURepresentationOfImage.h"

namespace ettention
{
    class NativeGPURepresentationOfImage : public GPURepresentationOfImage
    {
    public:
        NativeGPURepresentationOfImage(Vec3ui resolution, float *data);
        virtual ~NativeGPURepresentationOfImage();

        virtual size_t getSizeInBytes() override;
        virtual void* getGPURepresentation() override;
        virtual void convertBackToCPURepresentation(float*) override;

    protected:
        Vec3ui resolution;
        float* data;
    };
}
