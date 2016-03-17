__kernel void computeDetectorCao(__global float *minLambda, __global float *maxLambda, __global float *D, int sizeX, int sizeY)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);

	const unsigned int i = y * sizeX + x;
	float minL = minLambda[i];
	D[i] = minL * exp(minL - maxLambda[i]);
}