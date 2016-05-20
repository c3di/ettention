bool inside(int2 point, uint2 size)
{
	return (
		(point.x >= 0) &&
		(point.x < size.x) &&
		(point.y >= 0) &&
		(point.y < size.y)
	);
}

__kernel void computeLocalMax(__global float *src, __global float *result, int sizeX,  int sizeY, int width, int height)
{
	uint2 size 			= (uint2)((uint)sizeX, (uint)sizeY);
	uint2 workers 		= (uint2)(get_global_size(0), get_global_size(1));
	uint2 taskPerWorker	= (uint2)(size.x / workers.s0, size.y / workers.s1);
	uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));

	int2 pos 			= (int2)((int)workerId.x, (int)workerId.y);
	int currentIndex 	= pos.x + (pos.y * sizeX);
	float currentVal	= src[currentIndex];
	int2 radius			= (int2)(width / 2, height / 2);

	int2 windowBegin	= (int2)(pos.x - radius.x, pos.y - radius.y);
	int2 windowEnd		= (int2)(pos.x + radius.x, pos.y + radius.y);

	result[currentIndex] = 0.0f;

	if(windowBegin.x < 0)
		windowBegin.x = 0;

	if(windowBegin.y < 0)
		windowBegin.y = 0;

	if(windowEnd.x > sizeX)
		windowEnd.x = sizeX;

	if(windowEnd.y > sizeY)
		windowEnd.y = sizeY;

	// Finding local 1D Maxima in a row
	int index;
	float temp;

	for(int j = windowBegin.y; j < windowEnd.y; ++j)
	{
		index = pos.x + (j * sizeX);
		temp = src[index];

		if(temp > currentVal)
		{
			return;
		}
	}

	// For local 1D row maxima run spiral search
	int2 topHor;
	int2 botHor;
	int2 leftVert;
	int2 rightVert;

	for(int r = 1; r <= radius.x; ++r)
	{
		topHor		= (int2)(pos.x + r, pos.y + r);
		botHor		= (int2)(pos.x - r, pos.y - r);
		leftVert	= (int2)(pos.x - r, pos.y + r);
		rightVert	= (int2)(pos.x + r, pos.y - r);

		for(int i = 0; i < 2 * r; ++i)
		{
			topHor.x--;
			if(!inside(topHor, size))
				continue;

			index = topHor.x + (topHor.y * sizeX);
			temp = src[index];

			if(temp > currentVal)
			{
				return;
			}
		}
		for(int i = 0; i < 2 * r; ++i)
		{
			leftVert.y--;
			if(!inside(leftVert, size))
				continue;

			index = leftVert.x + (leftVert.y * sizeX);
			temp = src[index];

			if(temp > currentVal)
			{
				return;
			}
		}
		for(int i = 0; i < 2 * r; ++i)
		{
			botHor.x++;
			if(!inside(botHor, size))
				continue;

			index = botHor.x + (botHor.y * sizeX);
			temp = src[index];

			if(temp > currentVal)
			{
				return;
			}
		}
		for(int i = 0; i < 2 * r; ++i)
		{
			rightVert.y++;
			if(!inside(rightVert, size))
				continue;

			index = rightVert.x + (rightVert.y * sizeX);
			temp = src[index];

			if(temp > currentVal)
			{
				return;
			}
		}
	}
	result[currentIndex] = currentVal;
}