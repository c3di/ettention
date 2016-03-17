#pragma once
#include "framework/error/Exception.h"

namespace ettention
{
    class NoMemoryAllocatedForOutputException : public Exception 
    {
    public:
        NoMemoryAllocatedForOutputException(std::string sourceOfProblem);
    };
}
