#ifndef GENERATE_RAM_LAK_CL
#define GENERATE_RAM_LAK_CL

__kernel void generateKernel(uint resolution, __global float *kernelValues)
{
    const uint gid = (int) (uint) get_global_id(0);

    float value;

    int position = gid - resolution / 2;

    if (position == 0)
        value = 1.0f / 4.0f;
    else if ((position % 2) == 0)
        value = 0.0f;
    else
        value = -1.0f / (position*position*(float)(M_PI_F*M_PI_F));

    kernelValues[gid] = value;
}

#endif
