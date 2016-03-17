#include "stdafx.h"
#include "framework/error/GPUException.h"

#define CHECK_FOR_CL_ERROR(_errorCode, _refErrorCode) do \
{ \
    if(_errorCode == _refErrorCode) \
    { \
        return #_refErrorCode; \
    } \
} while(0)

namespace ettention
{
    GPUException::GPUException(cl_int errorCode, const char* file, unsigned int line, const char* command)
        : Exception("")
        , errorCode(errorCode)
    {
        std::stringstream messageStream;
        if(file)
        {
            messageStream << file << "(" << line << "): ";
        }
        else
        {
            messageStream << "Unknown source file: ";
        }
        if(command)
        {
            std::string cmdStr(command);
            auto parPos = cmdStr.find_first_of('(');
            if(parPos != cmdStr.npos)
            {
                messageStream << cmdStr.substr(0, parPos) << " returned ";
            }
        }
        messageStream << GetReadableCLError(errorCode);
        setMessage(messageStream.str());
    }

    GPUException::GPUException(const std::string& customMessage)
        : Exception(customMessage)
    {

    }

    GPUException::~GPUException()
    {

    }

    cl_int GPUException::GetErrorCode() const
    {
        return errorCode;
    }

    std::string GPUException::GetReadableCLError(cl_int errorCode)
    {
        CHECK_FOR_CL_ERROR(errorCode, CL_DEVICE_NOT_AVAILABLE);
        CHECK_FOR_CL_ERROR(errorCode, CL_DEVICE_NOT_FOUND);
        CHECK_FOR_CL_ERROR(errorCode, CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
        CHECK_FOR_CL_ERROR(errorCode, CL_IMAGE_FORMAT_NOT_SUPPORTED);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_ARG_INDEX);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_ARG_SIZE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_ARG_VALUE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_BINARY);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_BUFFER_SIZE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_BUILD_OPTIONS);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_COMMAND_QUEUE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_CONTEXT);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_DEVICE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_DEVICE_TYPE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_EVENT);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_EVENT_WAIT_LIST);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_GLOBAL_OFFSET);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_IMAGE_SIZE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_KERNEL);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_KERNEL_ARGS);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_KERNEL_DEFINITION);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_KERNEL_NAME);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_MEM_OBJECT);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_OPERATION);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_PLATFORM);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_PROGRAM);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_PROGRAM_EXECUTABLE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_PROPERTY);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_QUEUE_PROPERTIES);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_SAMPLER);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_VALUE);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_WORK_DIMENSION);
        CHECK_FOR_CL_ERROR(errorCode, CL_INVALID_WORK_GROUP_SIZE);
        CHECK_FOR_CL_ERROR(errorCode, CL_MEM_OBJECT_ALLOCATION_FAILURE);
        CHECK_FOR_CL_ERROR(errorCode, CL_OUT_OF_HOST_MEMORY);
        CHECK_FOR_CL_ERROR(errorCode, CL_OUT_OF_RESOURCES);
        CHECK_FOR_CL_ERROR(errorCode, CL_PROFILING_INFO_NOT_AVAILABLE);
        std::stringstream errStream;
        errStream << "Unknown error (code " << errorCode << ")";
        return errStream.str();
    }
}