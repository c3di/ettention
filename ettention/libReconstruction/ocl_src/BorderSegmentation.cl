__kernel void segmentBorderAndInternalArea(__global float *src, __global float *result, int sizeX,  int sizeY, int useAllEightNeighbors)
{
	const uint2 size 			= (uint2)((uint)sizeX, (uint)sizeY);
	const uint2 workers 		= (uint2)(get_global_size(0), get_global_size(1));
	const uint2 taskPerWorker	= (uint2)(size.x / workers.s0, size.y / workers.s1);
	const uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

	const int2 pos 				= (int2)((int)workerId.x, (int)workerId.y);
	const int currentIndex 		= pos.x + (pos.y * sizeX);

	result[currentIndex]  =  0.0f;
	if( src[currentIndex] <= 0.0f )		// External area
		return;

	if( (pos.x == 0) || (pos.x == (sizeX - 1) ) // pixels around the image edge are not part of border
	 || (pos.y == 0) || (pos.y == (sizeY - 1) )
	)
		return;

	bool missingOneOfFourNeighbors = false;
	missingOneOfFourNeighbors |= (src[pos.x + 1 + ( (pos.y)     * sizeX)] == 0.0f);
	missingOneOfFourNeighbors |= (src[pos.x - 1 + ( (pos.y)     * sizeX)] == 0.0f);
	missingOneOfFourNeighbors |= (src[pos.x     + ( (pos.y + 1) * sizeX)] == 0.0f);
	missingOneOfFourNeighbors |= (src[pos.x     + ( (pos.y - 1) * sizeX)] == 0.0f);

	bool missingOneOfAnotherFourNeighbors = false;
	missingOneOfAnotherFourNeighbors |= (src[pos.x + 1 + ( (pos.y + 1) * sizeX)] == 0.0f);
	missingOneOfAnotherFourNeighbors |= (src[pos.x - 1 + ( (pos.y + 1) * sizeX)] == 0.0f);
	missingOneOfAnotherFourNeighbors |= (src[pos.x + 1 + ( (pos.y - 1) * sizeX)] == 0.0f);
	missingOneOfAnotherFourNeighbors |= (src[pos.x - 1 + ( (pos.y - 1) * sizeX)] == 0.0f);

	bool missingAtLeastOneNeighbor = missingOneOfFourNeighbors;
	if( useAllEightNeighbors )
		missingAtLeastOneNeighbor |= missingOneOfAnotherFourNeighbors;

	if( missingAtLeastOneNeighbor )
		result[currentIndex] =-1.0f;	// Border
	else
		result[currentIndex] = 1.0f;	// Internal area
}