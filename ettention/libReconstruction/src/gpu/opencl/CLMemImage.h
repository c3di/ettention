#pragma once
#include "CLMemObject.h"
#include "CLImageProperties.h"

namespace ettention
{
    class CLMemImage : public CLMemObject
    {
    public:
        CLMemImage(CLAbstractionLayer* clal, const CLImageProperties& imgProps, const void* initialData = 0);
        ~CLMemImage();

        void copyTo(CLMemObject* dest) override;
        void transferTo(void* dest, std::size_t destByteWidth = 0) override;
        void transferFrom(const void* source, std::size_t sourceByteWidth = 0) override;
        const CLImageProperties& getProperties() const;

    private:
        CLImageProperties imgProps;
    };
}