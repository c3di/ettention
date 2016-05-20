#pragma once
#include <iostream>
#include <list>
#include <string>
#include "framework/error/Exception.h"
#include "framework/math/Vec3.h"

namespace ettention
{
    class VolumeResolutionDiffersException : public Exception 
    {
    public:
        VolumeResolutionDiffersException(Vec3ui expectedResolution, Vec3ui receivedResolution, std::string sourceOfProblem);
    };
}
