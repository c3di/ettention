#include "stdafx.h"
#include "CLMemObject.h"
#include "CLAbstractionLayer.h"
#include "CLImageProperties.h"
#include "CLKernel.h"
#include "framework/error/Exception.h"
#include "framework/time/PerformanceReport.h"

namespace ettention
{
    CLMemObject::CLMemObject(CLAbstractionLayer* clal, std::size_t byteWidth)
        : clal(clal)
        , clMem(0)
        , byteWidth(byteWidth)
    {
        clal->allocate(byteWidth);
    }

    CLMemObject::~CLMemObject()
    {
        clal->unregisterCLMem(clMem);
        clal->deallocate(byteWidth);
        CL_ASSERT(clReleaseMemObject(clMem));
    }

    const cl_mem& CLMemObject::getCLMem() const
    {
        return clMem;
    }

    void CLMemObject::setCLMem(cl_mem clMem)
    {
        if(this->clMem != 0)
        {
            throw GPUException("setCLMem() must only be called once!");
        }
        this->clMem = clMem;
        clal->registerCLMem(clMem);
    }

    std::size_t CLMemObject::getByteWidth() const
    {
        return byteWidth;
    }

    void CLMemObject::clear()
    {
        void* data = malloc(byteWidth);
        memset(data, 0, byteWidth);
        transferFrom(data);
        free(data);
    }
}