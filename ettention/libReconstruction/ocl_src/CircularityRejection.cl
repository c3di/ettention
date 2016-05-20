float computeCircularityScore(int perimeterPixelsCount, int innerAreaPixelsCount)
{
	if(perimeterPixelsCount == 0)
		return -1;

	// z = (4pi * A) / L^2, A - area, L - perimeter
	int A = innerAreaPixelsCount + perimeterPixelsCount;
	int L2 = perimeterPixelsCount * perimeterPixelsCount;
	return (float)A * (4 * M_PI) / L2;
}

__kernel void filterOutNonRoundRegions(__global float *particlesMap, __global float *segmentationMap, __global float *result, int sizeX, int sizeY, int particleSizeX, int particleSizeY)
{
	uint2 size 			= (uint2)((uint)sizeX, (uint)sizeY);
	uint2 workers 		= (uint2)(get_global_size(0), get_global_size(1));
	uint2 taskPerWorker	= (uint2)(size.x / workers.s0, size.y / workers.s1);
	uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

	int2 pos 			= (int2)((int)workerId.x, (int)workerId.y);
	int currentIndex 	= pos.x + (pos.y * sizeX);
	float currentValue	= particlesMap[currentIndex];

	if( currentValue <= 0.0f )
		return;

	uint2 imageSize 	= size;
	uint2 windowSize 	= (uint2)((uint)particleSizeX, (uint)particleSizeY);
	uint2 windowBegin 	= workerId;
	uint2 windowEnd 	= (uint2)(windowBegin.x + windowSize.x, windowBegin.y + windowSize.y);
	if( windowEnd.x > imageSize.x )					// Clamp to edge
	{
		windowEnd.x = imageSize.x;
	}
	if( windowEnd.y > imageSize.y )
	{
		windowEnd.y = imageSize.y;
	}

	int perimeter = 0;
	int inner = 0;

	uint index;
	float value;
	for(uint j = windowBegin.y; j < windowEnd.y; ++j)
	{
		for(uint i = windowBegin.x; i < windowEnd.x; ++i)
		{
			index = i + (imageSize.x * j);
			value = segmentationMap[index];
			if( value ==-1.0f)	// Border
			{
				++perimeter;
			} else 
			if( value == 1.0f)	// Inner Area
			{
				++inner;
			}
		}
	}

	float circularScore = computeCircularityScore(perimeter, inner);
	if( (circularScore <= 0.8f) || (circularScore >= 1.0f) )	// round enough for us
		currentValue = 0.0f;

	result[currentIndex] = currentValue;
}