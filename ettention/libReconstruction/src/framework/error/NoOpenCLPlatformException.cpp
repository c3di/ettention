#include "stdafx.h"
#include "NoOpenCLPlatformException.h"

namespace ettention
{
    NoOpenCLPlatformException::NoOpenCLPlatformException()
        : Exception("This PC does not provide a properly configured OpenCL platform!")
    {

    }

    NoOpenCLPlatformException::~NoOpenCLPlatformException()
    {

    }
}