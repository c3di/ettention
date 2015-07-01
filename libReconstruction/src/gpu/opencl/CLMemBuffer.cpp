#include "stdafx.h"
#include "CLMemBuffer.h"
#include "CLAbstractionLayer.h"
#include "CLMemImage.h"
#include "framework/NumericalAlgorithms.h"

namespace ettention
{
    CLMemBuffer::CLMemBuffer(CLAbstractionLayer* clal, std::size_t byteWidth, const void* initialData)
        : CLMemObject(clal, byteWidth)
    {
        cl_int err = 0;
        auto clMem = clCreateBuffer(clal->getContext(), CL_MEM_READ_WRITE, byteWidth, 0, &err);
        CL_ASSERT(err);
        setCLMem(clMem);
        if(initialData)
        {
            transferFrom(initialData);
        }
        else
        {
            clear();
        }
    }

    CLMemBuffer::~CLMemBuffer()
    {
    }

    void CLMemBuffer::transferFrom(const void* source)
    {
        cl_event evt = 0;
        CL_ASSERT(clEnqueueWriteBuffer(clal->getCommandQueue(), getCLMem(), CL_TRUE, 0, byteWidth, source, 0, 0, &evt));
        clal->profileAndReleaseEvent("Transfer from CPU to CLMemBuffer (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
    }

    void CLMemBuffer::transferFrom(const void* source, std::size_t offset, std::size_t byteWidth)
    {
        if(offset + byteWidth > this->byteWidth)
        {
            throw GPUException("Tried to write more data to CLMemBuffer than its capacity allows!");
        }
        cl_event evt = 0;
        CL_ASSERT(clEnqueueWriteBuffer(clal->getCommandQueue(), getCLMem(), CL_TRUE, offset, byteWidth, source, 0, 0, &evt));
        clal->profileAndReleaseEvent("Partial transfer from CPU to CLMemBuffer (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
    }

    void CLMemBuffer::transferTo(void* dest)
    {
        cl_event evt = 0;
        CL_ASSERT(clEnqueueReadBuffer(clal->getCommandQueue(), getCLMem(), CL_TRUE, 0, byteWidth, dest, 0, 0, &evt));
        clal->profileAndReleaseEvent("Transfer from CLMemBuffer to CPU (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
    }

    void CLMemBuffer::copyTo(CLMemObject* dest)
    {
        if(byteWidth != dest->getByteWidth())
        {
            throw GPUException("Bytewidths of CLMemBuffer and CLMemObject don't match!");
        }
        cl_event evt = 0;
        if(dynamic_cast<CLMemBuffer*>(dest))
        {
            CL_ASSERT(clEnqueueCopyBuffer(clal->getCommandQueue(), getCLMem(), dest->getCLMem(), 0, 0, byteWidth, 0, 0, &evt));
            clal->profileAndReleaseEvent("Copy from CLMemBuffer to CLMemBuffer (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
        }
        else if(dynamic_cast<CLMemImage*>(dest))
        {
            auto image = dynamic_cast<CLMemImage*>(dest);
            size_t origin[3] = { 0, 0, 0, };
            size_t region[3] = { image->getProperties().getResolution().x, image->getProperties().getResolution().y, image->getProperties().getResolution().z, };
            CL_ASSERT(clEnqueueCopyBufferToImage(clal->getCommandQueue(), getCLMem(), dest->getCLMem(), 0, origin, region, 0, 0, &evt));
            clal->profileAndReleaseEvent("Copy from CLMemBuffer to CLMemImage (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
        }
        else
        {
            throw GPUException(std::string("Copy from CLMemBuffer to ") + typeid(dest).name() + " not supported!");
        }
    }

    void CLMemBuffer::copyTo(CLMemBuffer* dest, std::size_t sourceOffset, std::size_t destOffset, std::size_t byteWidth)
    {
        cl_event evt = 0;
        CL_ASSERT(clEnqueueCopyBuffer(clal->getCommandQueue(), getCLMem(), dest->getCLMem(), sourceOffset, destOffset, byteWidth, 0, 0, &evt));
        clal->profileAndReleaseEvent("Partial copy from CLMemBuffer to CLMemBuffer (" + NumericalAlgorithms::bytesToString(byteWidth) + ")", evt);
    }
}