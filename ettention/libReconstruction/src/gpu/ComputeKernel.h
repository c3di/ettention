#pragma once
#include "framework/math/Vec2.h"

namespace ettention
{
    class CLAbstractionLayer;
    class CLKernel;
    class Framework;

    class ComputeKernel
    {
    public:
        ComputeKernel(Framework* framework, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, const std::string& additionalKernelDefines = "");
        virtual ~ComputeKernel();

        void run();

    protected:
        virtual void prepareKernelArguments() = 0;
        virtual void preRun() = 0;
        virtual void postRun() = 0;

        CLKernel* implementation;
        CLAbstractionLayer* abstractionLayer;
    };
}
