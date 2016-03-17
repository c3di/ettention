#include "stdafx.h"

#include "framework/error/ParameterNotFoundException.h"

namespace ettention
{
    ParameterNotFoundException::ParameterNotFoundException(std::string parameter)
        : Exception(std::string("parameter not found: ") + parameter), parameter(parameter)
    {
    }

    std::string ParameterNotFoundException::getParameter()
    {
        return parameter;
    }
}