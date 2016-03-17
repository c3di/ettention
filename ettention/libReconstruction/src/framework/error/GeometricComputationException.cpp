#include "stdafx.h"

#include "framework/error/GeometricComputationException.h"

namespace ettention
{

    GeometricComputationException::GeometricComputationException(std::string message)
        : Exception(std::string("error in geometric computation ") + message)
    {
    }
}