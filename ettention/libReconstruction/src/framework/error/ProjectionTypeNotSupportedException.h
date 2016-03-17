#pragma once

#include <iostream>
#include "framework/error/Exception.h"

namespace ettention
{
    class ProjectionTypeNotSupportedException : public Exception
    {
    public:
        ProjectionTypeNotSupportedException(const std::string& message);
        ~ProjectionTypeNotSupportedException();
    };
}
