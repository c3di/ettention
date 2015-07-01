#pragma once
#include "framework/math/Vec3.h"

namespace ettention 
{
    class GPURepresentationOfImage
    {
    public:
        GPURepresentationOfImage(Vec3ui resolution);
        virtual ~GPURepresentationOfImage();

        virtual Vec3ui getResolution();

        virtual size_t getSizeInBytes() = 0;
        virtual void* getGPURepresentation() = 0;
        virtual void convertBackToCPURepresentation(float*) = 0;

    protected:
        Vec3ui resolution;
    };
}
