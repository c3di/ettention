#ifndef MEAN_CL
#define MEAN_CL

__kernel void computeColumnAverage(__global float *src, __global float *colSumm, int sizeX,  int sizeY)
{
	uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

	if(workerId.x >= ((uint)sizeX) || workerId.y >= 1)
		return;

	int2 pos;
	pos.x = (int)workerId.x;
	
	int index = 0;
	float sum = 0.0f;
	for(int i = 0; i < sizeY; ++i)
	{
		pos.y = i;
		index = pos.x + (pos.y * sizeX);
		sum += src[index];
	}
	sum *= 1.0f / (sizeY);

	colSumm[pos.x] = sum;
}

#endif
