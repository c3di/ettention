#pragma once
#include "framework/geometry/BoundingBox.h"
#include "framework/math/Vec3.h"
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class VolumeParameterSet : public ParameterSet
    {
    private:
        Vec3ui resolution;
        Vec3f dimension;
        Vec3f origin;
        boost::filesystem::path initFile;
        float initValue;

    public:
        static const BoundingBox3f AUTOMATIC_BOUNDING_BOX;

        VolumeParameterSet(const ParameterSource* parameterSource);
        VolumeParameterSet(const Vec3ui& resolution, float initValue = 0.0f, const BoundingBox3f& boundingBox = AUTOMATIC_BOUNDING_BOX);
        virtual ~VolumeParameterSet();

        const Vec3ui& Resolution() const;
        const Vec3f& Origin() const;
        const Vec3f& Dimension() const;
        BoundingBox3f GetBoundingBox() const;
        float InitValue() const;
        const boost::filesystem::path& InitFile() const;
    };
}