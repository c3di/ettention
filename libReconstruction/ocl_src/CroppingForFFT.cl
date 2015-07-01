#ifndef CROPPING_CL
#define CROPPING_CL

__kernel void computeCropFromFourierSpace(__global float *src, __global float *result, int currentSizeX, int currentSizeY, int originalSizeX,  int originalSizeY)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);

	if (x >= originalSizeX || y >= originalSizeY)
		return;

	const unsigned int i = y * originalSizeX + x;

	int2 currentPos = (int2)(x, y);
	int2 sizeOld = (int2)(currentSizeX,		currentSizeY);
	int2 sizeNew = (int2)(originalSizeX,	originalSizeY);
	int2 sizeOldHalf = sizeOld / 2;
	int2 sizeNewHalf = sizeNew / 2;
	int2 centeredOriginalBegin = sizeOldHalf - sizeNewHalf;
	int2 pickPosition = currentPos + centeredOriginalBegin;

	int index = pickPosition.y * currentSizeX + pickPosition.x;
	result[i] = src[index];
}

#endif
