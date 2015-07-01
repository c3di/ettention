#include "stdafx.h"
#include "PatternByAveraging.h"

namespace ettention 
{
    Image* PatternByAveraging::computePattern(Image *src, std::vector<Vec2ui> initialGuess, int radius)
    {
        unsigned int side = (unsigned int)((radius * 2) + 1);
        Vec2ui patchResolution = Vec2ui(side, side);
        Image *result = new Image(patchResolution);
        
        int numberOfPatches = static_cast<int>(initialGuess.size());

        auto srcResolution = src->getResolution();
        
        Vec2ui point;
        float val;
        for(int k = 0; k < numberOfPatches; k++)
        {
            point = initialGuess.at(k);
            for(unsigned int j = 0; j < patchResolution.y; j++)
            {
                for(unsigned int i = 0; i < patchResolution.x; i++)
                {
                    val = src->getPixel(point.x + i, point.y + j) + result->getPixel(i, j);
                    result->setPixel(i, j, val);
                }
            }
        }

        for(unsigned int j = 0; j < patchResolution.y; j++)
        {
            for(unsigned int i = 0; i < patchResolution.x; i++)
            {
                val = result->getPixel(i, j);
                result->setPixel(i, j, val / numberOfPatches);
            }
        }

        return result;
    }
};