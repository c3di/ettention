#pragma once
#include "CLMemoryStructure.h"

namespace ettention
{
    class CLImageProperties;
    class CLMemImage;
    class CLSampler;

    class CLImage : public CLMemoryStructure
    {
    public:
        CLImage(CLAbstractionLayer* clal, const CLImageProperties& imgProps);
        ~CLImage();

        CLMemObject* getDataMemObject() const override;
        void setAsArgument(CLKernel* kernel, int argNumber) override;

    private:
        CLMemImage* data;
        CLSampler* sampler;
    };
}