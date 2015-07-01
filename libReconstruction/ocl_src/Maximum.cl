#ifndef MAX_CL
#define MAX_CL

#include "ImageUtils.hl"

__kernel void computeMax(__global float *src, __global int *colMaximumX, __global int *colMaximumY, __global int *maximumPosition, __global float *maximumValue, int sizeX,  int sizeY)
{
	uint2 size 			= (uint2)(sizeX, sizeY);
	uint2 workers 		= (uint2)(get_global_size(0), get_global_size(1));
	uint2 taskPerWorker	= (uint2)(size.x / workers.s0, size.y / workers.s1);
	uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

	if(workerId.x >= size.x || workerId.y >= 1)
		return;

	int2 pos 	= (int2)(workerId.x, 0);
	int2 maxPos = (int2)(workerId.x, 0);
	float maxVal= xsampler(src, size, maxPos);

	float temp;
	pos.x = workerId.x;
	for(int i = 1; i < size.y; ++i)
	{
		pos.y = i;
		temp = xsampler(src, size, pos);
		if(temp > maxVal)
		{
			maxPos = pos;
			maxVal = temp;
		}
	}

	colMaximumX[pos.x] = maxPos.x;
	colMaximumY[pos.x] = maxPos.y;

	//mem_fence(CLK_GLOBAL_MEM_FENCE);
	barrier(CLK_GLOBAL_MEM_FENCE);

	if(!isMainWorker(workerId))
	{
		return;
	}

	maxPos = (int2)(colMaximumX[0], colMaximumY[0]);
	maxVal = xsampler(src, size, maxPos);

	for(int j = 1; j < size.x; ++j)
	{
		pos  = (int2)(colMaximumX[j], colMaximumY[j]);
		temp = xsampler(src, size, pos);
		if(temp > maxVal)
		{
			maxPos = pos;
			maxVal = temp;
		}
	}

	maximumPosition[0] 	= maxPos.x;
	maximumPosition[1] 	= maxPos.y;
	maximumValue[0] 	= maxVal;
}

#endif
