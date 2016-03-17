#pragma once
#include <iostream>
#include <CL/cl.h>
#include "clAmdFft.h"
#include "framework/error/GPUException.h"

namespace ettention
{
    class FFTException : public GPUException
    {
    public:
        FFTException(clAmdFftStatus errorCode);
        ~FFTException();
    };
}
