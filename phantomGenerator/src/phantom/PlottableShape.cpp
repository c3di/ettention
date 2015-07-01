#include "stdafx.h"
#include "PlottableShape.h"
#include <model/volume/Volume.h>

namespace ettention
{
    PlottableShape::PlottableShape(Volume* volume, const Matrix4x4& m, float intensityDelta)
        : volume(volume)
        , transform(m)
        , intensityDelta(intensityDelta)
    {
        inverseTransform = transform.inverse();
    }

    PlottableShape::~PlottableShape()
    {
    }

    Volume* PlottableShape::getVolume() const
    {
        return volume;
    }

    void PlottableShape::plotToVolume(unsigned int sampleCount) const
    {
        const float intensityScale = 1.0f / (float)(sampleCount * sampleCount * sampleCount);

        Vec3f bb_from = getBoundingBox().GetMin();
        Vec3ui bb_from_i = Vec3ui((unsigned int)bb_from.x, (unsigned int)bb_from.y, (unsigned int)bb_from.z);

        Vec3f bb_to = getBoundingBox().GetMax();
        Vec3ui bb_to_i = Vec3ui((unsigned int)bb_to.x, (unsigned int)bb_to.y, (unsigned int)bb_to.z);

        Vec3ui position = bb_from_i;
        for(position.z = bb_from_i.z; position.z <= bb_to_i.z; position.z++)
        {
            for(position.y = bb_from_i.y; position.y <= bb_to_i.y; position.y++)
            {
                for(position.x = bb_from_i.x; position.x <= bb_to_i.x; position.x++)
                {
                    for(unsigned int samplePosX = 0; samplePosX < sampleCount; samplePosX++)
                    {
                        for(unsigned int samplePosY = 0; samplePosY < sampleCount; samplePosY++)
                        {
                            for(unsigned int samplePosZ = 0; samplePosZ < sampleCount; samplePosZ++)
                            {
                                const Vec3f subPos = Vec3f((float)samplePosX / (float)(sampleCount + 1),
                                                           (float)samplePosY / (float)(sampleCount + 1),
                                                           (float)samplePosZ / (float)(sampleCount + 1));

                                const Vec3f pointInObjectSpace = TransformCoord(inverseTransform, (Vec3f)position + subPos);
                                if(isPointInsideShape(pointInObjectSpace))
                                {
                                    writeShapeToPoint(position, intensityScale);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void PlottableShape::writeShapeToPoint(Vec3ui position, float intensityScale) const
    {
        if(position.x < 0 || position.y < 0 || position.z < 0)
        {
            return;
        }
        const Vec3ui volumeRes = volume->Properties().GetVolumeResolution();
        if(position.x >= (int)volumeRes.x || position.y >= (int)volumeRes.y || position.z >= (int)volumeRes.z)
        {
            return;
        }
        float intensity = volume->getVoxelValue(position);
        intensity += intensityDelta * intensityScale;
        volume->setVoxelToValue(position, intensity);
    }

    BoundingBox3f PlottableShape::getBoundingBox() const
    {
        return Transform(getObjectSpaceBoundingBox(), transform);
    }
}