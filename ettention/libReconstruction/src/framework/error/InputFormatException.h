#pragma once

#include <iostream>
#include "framework/error/Exception.h"

namespace ettention
{
    class InputFormatException : public Exception
    {
    public:
		InputFormatException(std::string message);
        std::string getParameter();

    protected:
        std::string parameter;
    };
}
