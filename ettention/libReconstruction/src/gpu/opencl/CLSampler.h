#pragma once
#include "CLImageProperties.h"
#include <unordered_set>

namespace ettention
{
    class CLAbstractionLayer;
    class CLKernel;

    class CLSampler
    {
    public:
        CLSampler(CLAbstractionLayer* clal, const CLImageProperties& imgProps);
        ~CLSampler();
        
        void setAsArgument(CLKernel* clKernel, unsigned int parameterNr);
        const cl_sampler& getCLSampler() const;

    private:
        CLAbstractionLayer* clal;
        CLImageProperties imgProps;
        cl_sampler clSampler;
    };
}