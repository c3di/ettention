#pragma once
#include "CLMemObject.h"

namespace ettention
{
    class CLMemBuffer : public CLMemObject
    {
    public:
        CLMemBuffer(CLAbstractionLayer* clal, std::size_t byteWidth, const void* initialData = 0);
        virtual ~CLMemBuffer();

        void copyTo(CLMemObject* dest) override;
        void copyTo(CLMemBuffer* dest, std::size_t sourceOffset, std::size_t destOffset, std::size_t byteWidth);
        void transferTo(void* dest) override;
        void transferFrom(const void* source) override;
        void transferFrom(const void* source, std::size_t offset, std::size_t byteWidth);
    };
}