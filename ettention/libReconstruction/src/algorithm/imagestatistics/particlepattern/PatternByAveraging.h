#pragma once
#include <vector>
#include "framework/math/Vec2.h"
#include "model/image/Image.h"

namespace ettention
{
    class PatternByAveraging
    {
    public:
        static Image *computePattern(Image *src, std::vector<Vec2ui> initialGuess, int radius);
    };
}