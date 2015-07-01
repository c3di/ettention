#pragma once
#include <iostream>
#include <list>
#include <string>
#include "framework/error/Exception.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class ImageResolutionDiffersException : public Exception 
    {
    public:
        ImageResolutionDiffersException (Vec2ui expectedResolution, Vec2ui receivedResolution, std::string sourceOfProblem);
    };
}
