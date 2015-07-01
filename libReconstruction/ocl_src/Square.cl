#ifndef SQUARE_CL
#define SQUARE_CL

__kernel void computeSquare(__global float *src, __global float *result, int sizeX,  int sizeY)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);

	if (x >= sizeX || y >= sizeY)
		return;

	const unsigned int i = y * sizeX + x;

	float val = src[i];
	result[i] = val * val;
}

#endif
