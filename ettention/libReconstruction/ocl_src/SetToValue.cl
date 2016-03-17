#ifndef SETTOVALUE_CL
#define SETTOVALUE_CL

__kernel void setToValue(__global float* volume,
                          uint volumeResolutionX,
                          uint volumeResolutionY,
                          uint volumeResolutionZ,
						  const float value)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
    const unsigned int z = get_global_id(2);

	if (x >= volumeResolutionX ||  y >= volumeResolutionY || z >= volumeResolutionZ)  
		return;

    const int voxelIndex = z * volumeResolutionX * volumeResolutionY + y * volumeResolutionX + x;
	
	volume[voxelIndex] = value;
}

#endif
