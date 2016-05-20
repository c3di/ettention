#pragma once
#include "framework/error/Exception.h"

namespace ettention
{
    class NoOpenCLPlatformException : public Exception
    {
    public:
        NoOpenCLPlatformException();
        ~NoOpenCLPlatformException();
    };
}