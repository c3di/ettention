#pragma once
#include "framework/geometry/BoundingBox.h"
#include "framework/math/Vec3.h"
#include "setup/parameterset/IOEnumerations.h"
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class VolumeParameterSet : public ParameterSet
    {
    public:
        static const BoundingBox3f AUTOMATIC_BOUNDING_BOX;

        VolumeParameterSet(const ParameterSource* parameterSource);
        VolumeParameterSet(const Vec3ui& resolution, float initValue = 0.0f, const BoundingBox3f& boundingBox = AUTOMATIC_BOUNDING_BOX);
        virtual ~VolumeParameterSet();

        const Vec3ui& getResolution() const;
        const Vec3f& getOrigin() const;
        const Vec3f& getDimension() const;
        BoundingBox3f getBoundingBox() const;
        float getInitValue() const;
        const boost::filesystem::path& getInitFile() const;
        CoordinateOrder getInitFileOrientation() const;

    private:
        Vec3ui resolution;
        Vec3f dimension;
        Vec3f origin;
        boost::filesystem::path initFile;
        float initValue;
        CoordinateOrder initFileOrientation;
    };
}