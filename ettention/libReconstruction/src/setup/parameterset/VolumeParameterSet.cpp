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

        resolution = Vec3ui( parameterSource->getVec3fParameter("volume.dimension") );
        if(parameterSource->parameterExists("volume.resolution"))
            resolution = parameterSource->getVec3uiParameter("volume.resolution");

        initValue = 0.0f;
        if(parameterSource->parameterExists("volume.initValue"))
        {
            initValue = parameterSource->getFloatParameter("volume.initValue");
        }

        initFile = "";
        if(parameterSource->parameterExists("volume.initFile"))
        {
            initFile = parameterSource->getPathParameter("volume.initFile");
        }

        initFileOrientation = ORDER_XZY;
        if(parameterSource->parameterExists("volume.initFileOrientation"))
        {
            initFileOrientation = parseVoxelCoordinateOrder(parameterSource->getStringParameter("volume.initFileOrientation"));
        }
    }

    VolumeParameterSet::VolumeParameterSet(const Vec3ui& resolution, float initValue, const BoundingBox3f& boundingBox)
        : dimension(&boundingBox == &AUTOMATIC_BOUNDING_BOX ? (Vec3f)resolution : boundingBox.getDimension())
        , origin(&boundingBox == &AUTOMATIC_BOUNDING_BOX ? Vec3f(0.0f, 0.0f, 0.0f) : boundingBox.getCenter())
        , resolution(resolution)
        , initValue(initValue)
        , initFile("")
    {
    }

    VolumeParameterSet::~VolumeParameterSet()
    {
    }

    const Vec3ui& VolumeParameterSet::getResolution() const
    {
        return resolution;
    }

    const Vec3f& VolumeParameterSet::getOrigin() const
    {
        return origin;
    }

    const Vec3f& VolumeParameterSet::getDimension() const
    {
        return dimension;
    }

    BoundingBox3f VolumeParameterSet::getBoundingBox() const
    {
        return BoundingBox3f(origin - 0.5f * dimension, origin + 0.5f * dimension);
    }

    float VolumeParameterSet::getInitValue() const
    {
        return initValue;
    }

    const boost::filesystem::path& VolumeParameterSet::getInitFile() const
    {
        return initFile;
    }

    CoordinateOrder VolumeParameterSet::getInitFileOrientation() const
    {
        return initFileOrientation;
    }
}