#pragma once
#include "OpenCLHeader.h"
#include "framework/math/Vec2.h"
#include "framework/math/Vec3.h"
#include <unordered_set>

namespace ettention
{
    class CLAbstractionLayer;
    class CLImageProperties;
    class CLKernel;

    class CLMemObject
    {
    public:
        CLMemObject(CLAbstractionLayer* clal, std::size_t byteWidth);
        virtual ~CLMemObject();

        const cl_mem& getCLMem() const;
        std::size_t getByteWidth() const;
        void clear();
        virtual void copyTo(CLMemObject* dest) = 0;
        virtual void transferTo(void* dest) = 0;
        virtual void transferFrom(const void* source) = 0;

    protected:
        CLAbstractionLayer* clal;
        const std::size_t byteWidth;

        void setCLMem(cl_mem clMem);

    private:
        cl_mem clMem;
    };
}