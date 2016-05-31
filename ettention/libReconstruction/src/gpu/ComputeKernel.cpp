#include "stdafx.h"
#include "ComputeKernel.h"
#include "framework/Framework.h"
#include "gpu/opencl/CLKernel.h"

namespace ettention
{
    ComputeKernel::ComputeKernel(Framework* framework, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, const std::string& additionalKernelDefines)
        : implementation(new CLKernel(framework, kernelCaption, sourceCode, kernelName, additionalKernelDefines))
        , abstractionLayer(framework->getOpenCLStack())
        , kernelName(kernelName)
    {
    }

    ComputeKernel::~ComputeKernel()
    {
        delete implementation;
    }

    void ComputeKernel::run()
    {
        prepareKernelArguments();
        preRun();
        implementation->run();
        postRun();
    }

    const std::string& ComputeKernel::getKernelName() const
    {
        return kernelName;
    }

}