#ifndef COMPARE_CL
#define COMPARE_CL

__kernel void accumulate (__global float* totalVolume,  __global float* deltaVolume, __global float* traversal_length, int volumeResolutionX, int volumeResolutionY, int volumeResolutionZ)
{
	const unsigned int x = get_global_id(0);
	const unsigned int y = get_global_id(1);

	if (x >= volumeResolutionX || y  >= volumeResolutionY )
		return;

	for (unsigned int z = 0; z < volumeResolutionZ; z++) 
	{
		const unsigned int i = z * volumeResolutionX * volumeResolutionY + y * volumeResolutionX + x;

		if (traversal_length[i] != 0.0f)
			deltaVolume[i] /= traversal_length[i]; 
		else 
			deltaVolume[i] = 0.0f;

		totalVolume[i] += deltaVolume[i];
	}
}

// <guid>

#endif
