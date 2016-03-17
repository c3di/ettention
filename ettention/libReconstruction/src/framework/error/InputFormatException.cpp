#include "stdafx.h"

#include "framework/error/InputFormatException.h"

namespace ettention
{
    InputFormatException::InputFormatException(std::string parameter)
        : Exception(std::string("config file format error near parameter: ") + parameter), parameter(parameter)
    {
    }

    std::string InputFormatException::getParameter()
    {
        return parameter;
    }
}