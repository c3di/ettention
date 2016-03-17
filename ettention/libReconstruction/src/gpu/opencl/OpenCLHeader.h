#pragma once

//2012_09_26: required by AMD APP v2.7 if OpenCL 1.1 is used
//2012_11_25: this is required behavior. The only "flaw" is that AMD APP 2.7 contains only 1.2 headers, not 1.1
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

#include <CL/cl.h>
#include "framework/error/GPUException.h"

#define CL_ASSERT(_clErr) do \
{ \
    const cl_int _result = _clErr; \
    if(_result != CL_SUCCESS) \
    { \
        throw GPUException(_result, __FILE__, __LINE__, #_clErr); \
    } \
} while(0)
