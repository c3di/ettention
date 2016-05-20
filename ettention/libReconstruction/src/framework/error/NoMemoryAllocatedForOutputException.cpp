#include "stdafx.h"
#include <iostream>
#include "NoMemoryAllocatedForOutputException.h"

namespace ettention 
{
    NoMemoryAllocatedForOutputException::NoMemoryAllocatedForOutputException(std::string sourceOfProblem)
    {
        std::stringstream messageStream;
        messageStream << "Trying to run kernel without allocation or specification of result container. Raised by: " << sourceOfProblem;
        setMessage(messageStream.str());
    }
}