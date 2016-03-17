#pragma once
#include "model/blobs/BlobParameters.h"
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class BlobParameters;
    class ParameterSource;

    class BlobParameterSet : public ParameterSet
    {
    public:
        BlobParameterSet(const ParameterSource* source);
        ~BlobParameterSet();

        float getSupportRadius() const;
        float getDensityFalloff() const;
        unsigned int getDegree() const;
        const BlobParameters& getBlobParameters() const;

    private:
        float supportRadius;
        float densityFalloff;
        unsigned int degree;
        BlobParameters blobParameters;
    };
}