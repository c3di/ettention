#include "stdafx.h"
#include "ComposedShape.h"
//#include <framework/Logger.h>

namespace ettention
{
    ComposedShape::ComposedShape(Volume* volume, const Matrix4x4& m)
        : PlottableShape(volume, m, 0.0f)
    {
    }

    ComposedShape::~ComposedShape()
    {
        for(auto it = shapes.begin(); it != shapes.end(); ++it)
        {
            delete *it;
        }
    }

    void ComposedShape::plotToVolume(unsigned int sampleCount) const
    {
        for(unsigned int i = 0; i < shapes.size(); ++i)
        {
            shapes[i]->plotToVolume(sampleCount);
            //LOGGER_INFO_FORMATTED("Shape " << i << " of " << shapes.size() << " plotted.");
        }
    }

    bool ComposedShape::isPointInsideShape(Vec3f p) const
    {
        for(auto it = shapes.begin(); it != shapes.end(); ++it)
        {
            if((*it)->isPointInsideShape(p))
            {
                return true;
            }
        }
        return false;
    }

    BoundingBox3f ComposedShape::getObjectSpaceBoundingBox() const
    {
        return BoundingBox3f(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 0.0f));
    }

    void ComposedShape::addShape(PlottableShape* shape)
    {
        shapes.push_back(shape);
    }
}