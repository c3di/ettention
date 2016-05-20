__kernel void computeThresholdWithHysteresis(__global float *src, __global float *result, int sizeX,  int sizeY, float floorLevel, float ceilingLevel)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	const unsigned int i = y * sizeX + x;

	float value = src[i];
	if(value < floorLevel)
	{
		value = THRESHOLDING_FLOORVALUE;
	} else 
	if(value > ceilingLevel)
	{
		value = THRESHOLDING_CEILVALUE;
	}

	result[i] = value;
}