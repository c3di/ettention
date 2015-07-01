#ifndef CONVOLUTION_CL
#define CONVOLUTION_CL

#include "Math.hl"

__kernel void computeConvolution(__global float *convolutionKernel, __global float *source, __global float *result, const int kernelSizeX, const int kernelSizeY, const int sourceSizeX, const int sourceSizeY, float weightSum)
{
    uint2 sourceSize 		= (uint2)((uint)sourceSizeX, (uint)sourceSizeY);
    uint2 resultPosition 	= (uint2)(get_global_id(0), get_global_id(1));

    if(resultPosition.x >= sourceSize.x || resultPosition.y >= sourceSize.y)
        return;

    uint resultIndex	= resultPosition.x + (sourceSize.x * resultPosition.y);
    uint2 kernelSize 	= (uint2)((uint)kernelSizeX, (uint)kernelSizeY);
    uint2 kernelRadius	= kernelSize / 2;

    result[resultIndex] = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE;// Reserve some value to detect errors

    uint2 sourcePosition;

    uint kernelIndex;
    uint sourceIndex;

    float kernelVal;
    float sourceVal;

    float accumulatedWeight = 0.0f;
    float sum = 0.0f;
    for(uint j = 0; j < kernelSize.y; ++j)
    {
        sourcePosition.y = resultPosition.y + j - kernelRadius.y;
        if((sourcePosition.y < 0) || (sourcePosition.y >= sourceSize.y))
            continue;

        for(uint i = 0; i < kernelSize.x; ++i)
        {
            sourcePosition.x = resultPosition.x + i - kernelRadius.x;
            if((sourcePosition.x < 0) || (sourcePosition.x >= sourceSize.x))
                continue;

            kernelIndex = i + (kernelSize.x * j);
            sourceIndex = sourcePosition.x + (sourceSize.x * sourcePosition.y);

            kernelVal  = convolutionKernel[kernelIndex];
            sourceVal  = source[sourceIndex];

            sum += (kernelVal * sourceVal);
            accumulatedWeight += kernelVal;
        }
    }

    if(weightSum)
    {
        sum *= (accumulatedWeight / weightSum); // Redistribute weights for clamped pixels
    }
    result[resultIndex] = sum;
}

#endif
