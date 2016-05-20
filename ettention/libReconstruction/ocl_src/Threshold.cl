__kernel void computeThreshold(__global float *src, __global float *result, int sizeX,  int sizeY, float threshold)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);
	const unsigned int i = y * sizeX + x;

	float val = src[i];
	if(val < threshold)
	{
		result[i] = 0.0f;
	} else {
		result[i] = val;
	}
}