#pragma once
#include "CLMemoryStructure.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class CLAbstractionLayer;
    class CLImageProperties;

    class CLStructuredBuffer : public CLMemoryStructure
    {
    public:
        CLStructuredBuffer(CLAbstractionLayer* clal, const CLImageProperties& imgProps);
        ~CLStructuredBuffer();

        CLMemObject* getDataMemObject() const override;
        void setAsArgument(CLKernel* kernel, int argNumber) override;

    private:
        struct BufferImageProperties
        {
            Vec3i resolution;
            int addressingMode;
        };

        BufferImageProperties properties;
        CLMemObject* dataMemObject;
        CLMemObject* propertiesMemObject;
    };
}