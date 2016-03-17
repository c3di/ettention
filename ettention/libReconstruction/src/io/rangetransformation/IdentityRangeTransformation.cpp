#include "stdafx.h"
#include "IdentityRangeTransformation.h"

namespace ettention
{
    IdentityRangeTransformation::IdentityRangeTransformation(Volume* volume)
        : RangeTransformation(volume)
    {
    }

    float IdentityRangeTransformation::transformRange(float value)
    {
        return value;
    }

    void IdentityRangeTransformation::transformRange(float* data, size_t size)
    {
    }

};
