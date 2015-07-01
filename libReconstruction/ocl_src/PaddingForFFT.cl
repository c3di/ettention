#ifndef PADDING_CL
#define PADDING_CL

__kernel void computeZeroPaddedCopyForFourierSpace(__global float *src, __global float *result, int originalSizeX,  int originalSizeY, int desiredSizeX, int desiredSizeY)
{
	const int2 pos = (int2)((int)get_global_id(0), (int)get_global_id(1));
	if (pos.x >= desiredSizeX || pos.y >= desiredSizeY)
		return;

	const int i = pos.y * desiredSizeX + pos.x;

	int2 centeredOriginalBegin = ((int2)(desiredSizeX, desiredSizeY) - (int2)(originalSizeX, originalSizeY)) / 2;
	int2 centeredOriginalEnd = ((int2)(desiredSizeX, desiredSizeY) + (int2)(originalSizeX, originalSizeY)) / 2;
	int2 sourcePickPosition = pos - centeredOriginalBegin;

	float val = 0.0f;
	if((pos.x >= centeredOriginalBegin.x) && (pos.x < centeredOriginalEnd.x) 
	 && (pos.y >= centeredOriginalBegin.y) && (pos.y < centeredOriginalEnd.y))
		val = src[sourcePickPosition.y * originalSizeX + sourcePickPosition.x];

	result[i] = val;
}

__kernel void computePaddedCopyForFourierSpace(__global float *src, __global float *result, int originalSizeX,  int originalSizeY, int desiredSizeX, int desiredSizeY)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);

	if (x >= desiredSizeX || y >= desiredSizeY)
		return;

	const unsigned int i = y * desiredSizeX + x;

	int2 currentPos = (int2)(x, y);
	int2 sizeOld = (int2)(originalSizeX, originalSizeY);
	int2 centeredOriginalBegin = ((int2)(desiredSizeX, desiredSizeY) - sizeOld) / 2;
	int2 centeredOriginalEnd = ((int2)(desiredSizeX, desiredSizeY) + sizeOld) / 2;

	bool xInside = (x >= centeredOriginalBegin.x) && (x < centeredOriginalEnd.x);
	bool yInside = (y >= centeredOriginalBegin.y) && (y < centeredOriginalEnd.y);

	int2 sourcePickPosition = currentPos - centeredOriginalBegin;
	if(!yInside)
	{
		if(y < centeredOriginalEnd.y)
			sourcePickPosition.y = centeredOriginalBegin.y - currentPos.y;
		else
			sourcePickPosition.y = (sizeOld.y + centeredOriginalEnd.y - 1) - currentPos.y;
	}

	if(!xInside)
	{
		if(x < centeredOriginalEnd.x)
			sourcePickPosition.x = centeredOriginalBegin.x - currentPos.x;
		else
			sourcePickPosition.x = (sizeOld.x + centeredOriginalEnd.x - 1) - currentPos.x;
	}

	int index = sourcePickPosition.y * originalSizeX + sourcePickPosition.x;
	result[i] = src[index];
}

#endif
