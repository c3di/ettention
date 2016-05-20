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

	if(windowEnd.x > (sizeX - 1))
		windowEnd.x = (sizeX - 1);

	if(windowEnd.y > (sizeY - 1))
		windowEnd.y = (sizeY - 1);

	int index;
	float temp;

	for(int j = windowBegin.y; j <= windowEnd.y; ++j)
	{
		for(int i = windowBegin.x; i <= windowEnd.x; ++i)
		{
			index = i + (j * sizeX);
			temp = src[index];

			if(temp > currentVal)
			{
				return;
			}
		}
	}
	result[currentIndex] = currentVal;
}
