#include "Math.hl"

__kernel void computeCycledNCC(__global float *reference, __global float *candidate, __global float *result, const int referenceSizeX, const int referenceSizeY, const int candidateSizeX, const int candidateSizeY, float candidateMean)
{
    const uint2 imageSize 		= (uint2)((uint)referenceSizeX, (uint)referenceSizeY);
    const uint2 workers 		= (uint2)(get_global_size(0), get_global_size(1));
    const uint2 taskPerWorker	= (uint2)(imageSize.x / workers.s0, imageSize.y / workers.s1);
    const uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

    uint2 windowSize 			= (uint2)((uint)candidateSizeX, (uint)candidateSizeY);

	const uint currentIndex 	= Flatten2D(workerId, imageSize);
    result[currentIndex] 		= THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE;

    uint u = workerId.x;	// sugar
    uint v = workerId.y;	// sugar

    uint2 windowBegin			= (uint2)(u, v);
    uint2 windowEnd				= (uint2)(u + windowSize.x, v + windowSize.y);

    uint refIndex;
    uint cndIndex;

    // Compute Mean of region under feature (inside Window))
    float referenceWindowMean = 0.0f;
    for(uint j = windowBegin.y; j < windowEnd.y; ++j)
    {
        for(uint i = windowBegin.x; i < windowEnd.x; ++i)
        {
            refIndex = (i % imageSize.x) + (imageSize.x * (j % imageSize.y));	// WRAP AROUND
            referenceWindowMean += reference[refIndex];
        }
    }
    referenceWindowMean *= 1.0f / (windowSize.x * windowSize.y);

    float referenceVal;
    float candidateVal;

    float sum = 0.0f;
    float referenceWindowDev = 0.0f;
    float candidateWindowDev = 0.0f;
    for(uint j = windowBegin.y; j < windowEnd.y; ++j)
    {
        for(uint i = windowBegin.x; i < windowEnd.x; ++i)
        {
            refIndex = (i % imageSize.x) + (imageSize.x * (j % imageSize.y));	// WRAP AROUND
            cndIndex = (i - u) + (windowSize.x * (j - v));

            referenceVal  = reference[refIndex] - referenceWindowMean;
            candidateVal  = candidate[cndIndex] - candidateMean;

            sum += (referenceVal * candidateVal);

            referenceWindowDev += (referenceVal * referenceVal);
            candidateWindowDev += (candidateVal * candidateVal);
        }
    }

    // Prevent division by zero
    if( (referenceWindowDev == 0.0f) || (candidateWindowDev == 0.0f) )
    {
        sum = -1.0f;
    } else {
        sum *= 1.0f / sqrt(referenceWindowDev);
        sum *= 1.0f / sqrt(candidateWindowDev);
    }
    result[currentIndex] = sum;
}
