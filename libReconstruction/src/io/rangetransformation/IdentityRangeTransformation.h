#pragma once
#include "io/rangetransformation/RangeTransformation.h"

namespace ettention
{
    class Volume;

    class IdentityRangeTransformation : public RangeTransformation
    {
    public:
        IdentityRangeTransformation(Volume* volume);

        virtual float transformRange(float value) override;
        virtual void transformRange(float* data, size_t size) override;
    };
};
