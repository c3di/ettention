#include "Math.hl"

__kernel void computeDilation(__global float *src, __global float *structElement, __global float *result, int sizeX,  int sizeY, int structWidth, int structHeight)
{
	uint2 size 			= (uint2)((uint)sizeX, (uint)sizeY);
	uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

	uint currentIndex 	= Flatten2D(workerId, size);
	float currentVal	= src[currentIndex];

	int2 pos 			= (int2)((int)workerId.x, (int)workerId.y);
	int2 radius			= (int2)(structWidth / 2, structHeight / 2);

	int2 srcPos;
	int srcIndex;
	int structIndex;

	float maxVal = currentVal;
	float srcVal;
	float structVal;
	float temp;

	for(int j = 0; j < structHeight; ++j)
	{
		srcPos.y = pos.y + j - radius.y;
		if((srcPos.y < 0) || (srcPos.y >= sizeY))
			continue;

		for(int i = 0; i < structWidth; ++i)
		{
			srcPos.x = pos.x + i - radius.x;
			if((srcPos.x < 0) || (srcPos.x >= sizeX))
				continue;

			srcIndex = srcPos.x + (srcPos.y * sizeX);
			structIndex = i + (j * structWidth);

			srcVal = src[srcIndex];
			structVal = structElement[structIndex];

			temp = srcVal * structVal;
			if( (structVal > 0) && (temp > maxVal) )
			{
				maxVal = temp;
			}
		}
	}
	result[currentIndex] = maxVal;
}
