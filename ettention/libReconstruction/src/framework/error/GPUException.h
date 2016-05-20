#pragma once
#include <iostream>
#include <CL/cl.h>
#include "framework/error/Exception.h"

namespace ettention
{
    class GPUException : public Exception
    {
    public:
        GPUException(cl_int errorCode, const char* file = 0, unsigned int line = 0, const char* command = 0);
        GPUException(const std::string& customMessage);
        virtual ~GPUException();

        static std::string GetReadableCLError(cl_int errorCode);

    protected:
        cl_int GetErrorCode() const;

    private:
        cl_int errorCode;
    };
}
