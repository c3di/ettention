#pragma once
#include "algorithm/longobjectcompensation/LongObjectCompensationKernel.h"

namespace ettention
{
    class InputProjectionKernel : public LongObjectCompensationKernel
    {
    public:
        InputProjectionKernel(Framework* framework, GPUMapped<Image>* traversalLength);
        ~InputProjectionKernel();
    };
}