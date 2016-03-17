__kernel void computeMean(__global float *src, __global float *result, int size)
{
	uint2 workerId 		= (uint2)(get_global_id(0), get_global_id(1));
	
	if(workerId.x >= 1 || workerId.y >= 1)
		return;

	float sum = 0.0f;
	for(int i = 0; i < size; ++i)
	{
		sum += src[i];
	}
	sum *= 1.0f / (size);

	result[0] = sum;
}