#include "stdafx.h"
#include "framework/error/FFTException.h"

namespace ettention
{
    std::string GetReadableError(clAmdFftStatus errorCode);

    FFTException::FFTException(clAmdFftStatus errorCode)
        : GPUException(GetReadableError(errorCode))
    {

    }

    FFTException::~FFTException()
    {

    }

    std::string GetReadableError(clAmdFftStatus errorCode)
    {
        switch(errorCode)
        {
        case CLFFT_BUGCHECK:
            return "Bugcheck.";
        case CLFFT_NOTIMPLEMENTED:
            return "Functionality is not implemented yet.";
        case CLFFT_TRANSPOSED_NOTIMPLEMENTED:
            return "Transposed functionality is not implemented for this transformation.";
        case CLFFT_FILE_NOT_FOUND:
            return "Tried to open an existing file on the host system, but failed.";
        case CLFFT_FILE_CREATE_FAILURE:
            return "Tried to create a file on the host system, but failed.";
        case CLFFT_VERSION_MISMATCH:
            return "Version conflict between client and library.";
        case CLFFT_INVALID_PLAN:
            return "Requested plan could not be found.";
        case CLFFT_DEVICE_NO_DOUBLE:
            return "Double precision not supported on this device.";
        case CLFFT_DEVICE_MISMATCH:
            return "Attempt to run on a device using a plan baked for a different device.";
        case CLFFT_ENDSTATUS:
            return "Endstatus (should not happen!)";
        default:
            return GPUException::GetReadableCLError(errorCode);
        }
    }
}