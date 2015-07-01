#include "stdafx.h"
#include "InputProjectionKernel.h"
#include "InputProjection_bin2c.h"

namespace ettention
{
    InputProjectionKernel::InputProjectionKernel(Framework* framework, GPUMapped<Image>* traversalLength)
        : LongObjectCompensationKernel(framework, InputProjection_kernel_SourceCode, "compensate", "InputProjectionKernel", traversalLength)
    {
    }

    InputProjectionKernel::~InputProjectionKernel()
    {
    }
}