#pragma once

#include <iostream>
#include "framework/error/Exception.h"

namespace ettention
{
    class GeometricComputationException : public Exception
    {
    public:
        GeometricComputationException(std::string message);
    };
}