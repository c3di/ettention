#include "stdafx.h"
#include "algorithm/longobjectcompensation/VirtualProjectionKernel.h"
#include "VirtualProjection_bin2c.h"

namespace ettention
{
    VirtualProjectionKernel::VirtualProjectionKernel(Framework* framework, GPUMapped<Image>* projection, GPUMapped<Image>* traversalLength, boost::optional<float> constantLength)
        : LongObjectCompensationKernel(framework, VirtualProjection_kernel_SourceCode, "compensate", "VirtualProjectionKernel", projection, traversalLength, constantLength)
    {
    }

    VirtualProjectionKernel::~VirtualProjectionKernel()
    {
    }
}