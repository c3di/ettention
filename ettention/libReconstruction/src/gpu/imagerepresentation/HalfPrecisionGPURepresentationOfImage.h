#pragma once

#include "gpu/imagerepresentation/GPURepresentationOfImage.h"
#include "CL/cl.h"

namespace ettention
{
    class HalfPrecisionGPURepresentationOfImage : public GPURepresentationOfImage
    {
    public:
        HalfPrecisionGPURepresentationOfImage(Vec3ui resolution, float *data);
        virtual ~HalfPrecisionGPURepresentationOfImage();

        virtual size_t getSizeInBytes() override;
        virtual void* getGPURepresentation() override;
        virtual void convertBackToCPURepresentation(float*) override;

    protected:
        Vec3ui resolution;
        std::vector<cl_half> gpudata;
    };
}
