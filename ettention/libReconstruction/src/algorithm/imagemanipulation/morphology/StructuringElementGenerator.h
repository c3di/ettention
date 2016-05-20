#pragma once
#include "framework/math/Vec2.h"

namespace ettention
{
    class Image;

    class StructuringElementGenerator
    {
        StructuringElementGenerator() = delete;

    public:
        static Image *generateDiamondElement();

    protected:
        static Image *generateElementByPattern(Vec2ui resolution, const float pattern[]);

    private:
        static const float DIAMOND_ELEMENT_ARRAY[25];
    };
}