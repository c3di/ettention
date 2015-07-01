#include "stdafx.h"
#include "algorithm/longobjectcompensation/VirtualProjectionKernel.h"
#include "VirtualProjection_bin2c.h"

namespace ettention
{
    VirtualProjectionKernel::VirtualProjectionKernel(Framework* framework, GPUMapped<Image>* traversalLength)
        : LongObjectCompensationKernel(framework, VirtualProjection_kernel_SourceCode, "compensate", "VirtualProjectionKernel", traversalLength)
    {
    }

    VirtualProjectionKernel::~VirtualProjectionKernel()
    {
    }
}