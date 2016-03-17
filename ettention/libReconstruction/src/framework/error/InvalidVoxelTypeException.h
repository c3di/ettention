#pragma once

#include <iostream>
#include "framework/error/Exception.h"

namespace ettention
{
    class InvalidVoxelTypeException : public Exception
    {
    public:
        InvalidVoxelTypeException();
        ~InvalidVoxelTypeException();
    };
}
