#include "stdafx.h"

#include "framework/error/ImageResolutionDiffersException.h"

namespace ettention 
{
    ImageResolutionDiffersException::ImageResolutionDiffersException(Vec2ui expectedResolution, Vec2ui receivedResolution, std::string sourceOfProblem)
    {
        std::stringstream messageStream;
        messageStream << "Kernel was initialized for input images of different resolution, and was not designed to reallocate memory. ";
        messageStream << "Allocated for output: " << expectedResolution.x << "x" << expectedResolution.y << ". ";
        messageStream << "Received as input: " << receivedResolution.x << "x" << receivedResolution.y << ".";
        messageStream << "Caused by: " + sourceOfProblem << ".";
        setMessage(messageStream.str());
    }
}