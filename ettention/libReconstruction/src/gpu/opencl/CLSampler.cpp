#include "stdafx.h"
#include "CLSampler.h"
#include "CLAbstractionLayer.h"
#include "CLKernel.h"

namespace ettention
{
    CLSampler::CLSampler(CLAbstractionLayer* clal, const CLImageProperties& imgProps)
        : clal(clal)
    {
        cl_int err = 0;
        clSampler = clCreateSampler(clal->getContext(), imgProps.getNormalizedCoordinates(), imgProps.getAddressingMode(), imgProps.getFilterMode(), &err);
        CL_ASSERT(err);
        clal->registerCLSampler(clSampler);
    }

    CLSampler::~CLSampler()
    {
        clal->unregisterCLSampler(clSampler);
        CL_ASSERT(clReleaseSampler(clSampler));
    }

    void CLSampler::setAsArgument(CLKernel* clKernel, unsigned int parameterNr)
    {
        CL_ASSERT(clSetKernelArg(clKernel->getKernel(), parameterNr, sizeof(cl_sampler), &clSampler));
    }

    const cl_sampler& CLSampler::getCLSampler() const
    {
        return clSampler;
    }
}