#pragma once

namespace ettention 
{
    class CLAbstractionLayer;
    class CLImageProperties;
    class CLKernel;
    class CLMemObject;

    class CLMemoryStructure
    {
    public:
        CLMemoryStructure();
        virtual ~CLMemoryStructure();

        virtual CLMemObject* getDataMemObject() const = 0;
        virtual void setAsArgument(CLKernel* kernel, int argNumber) = 0;

        static CLMemoryStructure* create(CLAbstractionLayer* clal, const CLImageProperties& imgProps);
    };
}