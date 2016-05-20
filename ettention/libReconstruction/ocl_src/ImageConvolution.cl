#include "Math.hl"

__kernel void computeConvolution(__global float *convolutionKernel, __global float *source, __global float *result, const int kernelSizeX, const int kernelSizeY, const int sourceSizeX, const int sourceSizeY, float weightSum)
{
	uint2 size 			= (uint2)((uint)sourceSizeX, (uint)sourceSizeY);
	uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

	uint currentIndex 	= Flatten2D(workerId, size);
	float currentVal	= THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE;

	int2 pos 			= (int2)((int)workerId.x, (int)workerId.y);
	int2 radius			= (int2)(kernelSizeX / 2, kernelSizeY / 2);

	int2 sourcePosition;
	int sourceIndex;
	int kernelIndex;

	float kernelVal;
	float sourceVal;

	float accumulatedWeight = 0.0f;
	float sum = 0.0f;
	for(int j = 0; j < kernelSizeY; ++j)
	{
		sourcePosition.y = pos.y + j - radius.y;
		if((sourcePosition.y < 0) || (sourcePosition.y >= sourceSizeY))
			continue;

		for(int i = 0; i < kernelSizeX; ++i)
		{
			sourcePosition.x = pos.x + i - radius.x;
			if((sourcePosition.x < 0) || (sourcePosition.x >= sourceSizeX))
				continue;

			kernelIndex = i + (kernelSizeX * j);
			sourceIndex = sourcePosition.x + (sourceSizeX * sourcePosition.y);

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
	currentVal = sum;

	result[currentIndex] = currentVal;
}