#include "stdafx.h"
#include "VolumeParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    const BoundingBox3f VolumeParameterSet::AUTOMATIC_BOUNDING_BOX(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f));

    VolumeParameterSet::VolumeParameterSet(const ParameterSource* parameterSource)
    {
        dimension = parameterSource->getVec3fParameter("volume.dimension");

        origin = Vec3f(0.0f, 0.0f, 0.0f);
        if(parameterSource->parameterExists("volume.origin"))
            origin = parameterSource->getVec3fParameter("volume.origin");

        resolution = parameterSource->getVec3uiParameter("volume.dimension");
        if(parameterSource->parameterExists("volume.resolution"))
            resolution = parameterSource->getVec3uiParameter("volume.resolution");

        initValue = 0.0f;
        if(parameterSource->parameterExists("volume.initValue"))
        {
            initValue = parameterSource->getFloatParameter("volume.initValue");
        }
        else if(parameterSource->parameterExists("volume.initFile"))
        {
            initFile = parameterSource->getPathParameter("volume.initFile");
        }
        else
        {
            throw Exception("Neither volume.initValue nor volume.initFile was specified!");
        }
    }

    VolumeParameterSet::VolumeParameterSet(const Vec3ui& resolution, float initValue, const BoundingBox3f& boundingBox)
        : dimension(&boundingBox == &AUTOMATIC_BOUNDING_BOX ? (Vec3f)resolution : boundingBox.GetDimension())
        , origin(&boundingBox == &AUTOMATIC_BOUNDING_BOX ? Vec3f(0.0f, 0.0f, 0.0f) : boundingBox.GetCenter())
        , resolution(resolution)
        , initValue(initValue)
        , initFile("")
    {
    }

    VolumeParameterSet::~VolumeParameterSet()
    {
    }

    const Vec3ui& VolumeParameterSet::Resolution() const
    {
        return resolution;
    }

    const Vec3f& VolumeParameterSet::Origin() const
    {
        return origin;
    }

    const Vec3f& VolumeParameterSet::Dimension() const
    {
        return dimension;
    }

    BoundingBox3f VolumeParameterSet::GetBoundingBox() const
    {
        return BoundingBox3f(origin - 0.5f * dimension, origin + 0.5f * dimension);
    }

    float VolumeParameterSet::InitValue() const
    {
        return initValue;
    }

    const boost::filesystem::path& VolumeParameterSet::InitFile() const
    {
        return initFile;
    }
}