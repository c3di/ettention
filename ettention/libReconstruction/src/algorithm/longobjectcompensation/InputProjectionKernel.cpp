#include "stdafx.h"
#include "InputProjectionKernel.h"
#include "InputProjection_bin2c.h"

namespace ettention
{
    InputProjectionKernel::InputProjectionKernel(Framework* framework, GPUMapped<Image>* projection, GPUMapped<Image>* traversalLength, boost::optional<float> constantLength)
        : LongObjectCompensationKernel(framework, InputProjection_kernel_SourceCode, "compensate", "InputProjectionKernel", projection, traversalLength, constantLength)
    {
    }

    InputProjectionKernel::~InputProjectionKernel()
    {
    }
}