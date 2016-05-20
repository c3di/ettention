#include "stdafx.h"
#include "CLImage.h"
#include "CLKernel.h"
#include "CLMemImage.h"
#include "CLSampler.h"

namespace ettention
{
    CLImage::CLImage(CLAbstractionLayer* clal, const CLImageProperties& imgProps)
    {
        data = new CLMemImage(clal, imgProps);
        sampler = new CLSampler(clal, imgProps);
    }

    CLImage::~CLImage()
    {
        delete sampler;
        delete data;
    }

    CLMemObject* CLImage::getDataMemObject() const
    {
        return data;
    }

    void CLImage::setAsArgument(CLKernel* kernel, int argNumber)
    {
        kernel->setArgument(argNumber, data);
        kernel->setArgument(argNumber + 1, sampler);
    }
}