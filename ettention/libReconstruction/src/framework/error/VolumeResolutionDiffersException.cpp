#include "stdafx.h"

#include "framework/error/VolumeResolutionDiffersException.h"

namespace ettention 
{
    VolumeResolutionDiffersException::VolumeResolutionDiffersException(Vec3ui expectedResolution, Vec3ui receivedResolution, std::string sourceOfProblem)
    {
        std::stringstream messageStream;
        messageStream << "Kernel was initialized for input images of different resolution, and was not designed to reallocate memory. ";
        messageStream << "Allocated for output: " << expectedResolution << ". ";
        messageStream << "Received as input: " << receivedResolution << ".";
        messageStream << "Caused by: " + sourceOfProblem << ".";
        setMessage(messageStream.str());
    }
}