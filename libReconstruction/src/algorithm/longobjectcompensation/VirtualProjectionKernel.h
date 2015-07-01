#pragma once
#include "algorithm/longobjectcompensation/LongObjectCompensationKernel.h"

namespace ettention
{
    class VirtualProjectionKernel : public LongObjectCompensationKernel
    {
    public:
        VirtualProjectionKernel(Framework* framework, GPUMapped<Image>* traversalLength);
        ~VirtualProjectionKernel();
    };
}