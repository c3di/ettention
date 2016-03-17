#include "stdafx.h"
#include "BlobParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    BlobParameterSet::BlobParameterSet(const ParameterSource* source)
        : blobParameters(BlobParameters::createDefault())
    {
        supportRadius = 2.0f;
        if(source->parameterExists("blobs.supportRadius"))
        {
            supportRadius = source->getFloatParameter("blobs.supportRadius");
        }

        degree = 2;
        if(source->parameterExists("blobs.degree"))
        {
            degree = source->getUIntParameter("blobs.degree");
        }

        densityFalloff = 3.6f;
        if(source->parameterExists("blobs.densityFalloff"))
        {
            densityFalloff = source->getFloatParameter("blobs.densityFalloff");
        }

        blobParameters = BlobParameters::createWithParameters(supportRadius, degree, densityFalloff);
    }

    BlobParameterSet::~BlobParameterSet()
    {
    }

    float BlobParameterSet::getSupportRadius() const
    {
        return supportRadius;
    }

    unsigned int BlobParameterSet::getDegree() const
    {
        return degree;
    }

    float BlobParameterSet::getDensityFalloff() const
    {
        return densityFalloff;
    }

    const BlobParameters& BlobParameterSet::getBlobParameters() const
    {
        return blobParameters;
    }
}