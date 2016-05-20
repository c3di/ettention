#pragma once

#include "io/rangetransformation/RangeTransformation.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class LinearRangeTransformation : public RangeTransformation
    {
    public:
        LinearRangeTransformation(Volume* volume, float outputRangeMinimum, float outputRangeMaximum);

        virtual float transformRange(float value) override;
        virtual void transformRange(float* data, size_t size) override;

    protected:
        float outMin;
        float outMax;
    };
};
