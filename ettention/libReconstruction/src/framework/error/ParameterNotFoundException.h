#pragma once

#include <iostream>
#include "framework/error/Exception.h"

namespace ettention
{
    class ParameterNotFoundException : public Exception
    {
    public:
        ParameterNotFoundException(std::string message);
        std::string getParameter();

    protected:
        std::string parameter;
    };
}
