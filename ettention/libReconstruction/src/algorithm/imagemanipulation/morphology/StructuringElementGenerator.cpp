#include "stdafx.h"
#include "StructuringElementGenerator.h"

#include "model/image/Image.h"
#include "framework/error/Exception.h"

namespace ettention
{
    const float StructuringElementGenerator::DIAMOND_ELEMENT_ARRAY[] = {
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        1, 1, 1, 1, 1,
        0, 1, 1, 1, 0,
        0, 0, 1, 0, 0
    };

    Image *StructuringElementGenerator::generateDiamondElement()
    {
        return generateElementByPattern(Vec2ui(5, 5), DIAMOND_ELEMENT_ARRAY);
    }

    Image *StructuringElementGenerator::generateElementByPattern(Vec2ui resolution, const float pattern[])
    {
        auto *result = new Image(resolution, pattern);
        return result;
    }
}