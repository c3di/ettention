void accumulateColumnWise(uint x, uint2 resolution, __global float* residual, __global float* result)
{
    float column_rms = 0.0f;
    for(uint y = 0; y < resolution.y; y++)
    {
        const uint i = y * resolution.x + x;
        column_rms += residual[i] * residual[i];
    }
    result[x] = column_rms;
}

float accumulatePartialResults(uint sizeX, __global float* result)
{
    float total_rms = 0.0f;
    for(unsigned int x = 0; x < sizeX; x++)
    {
        total_rms += result[x];
    }
    return total_rms;
}

__kernel void compute(uint2 resolution, __global float* residual, __global float* result)
{
    const uint workGroupId = get_global_id(0);
    const uint columsPerThread = resolution.x / get_local_size(0);
    const uint threadOffset = workGroupId * columsPerThread;

    for(uint i = 0; i < columsPerThread; i++)
    {
        uint x = threadOffset + i;
        if(x >= resolution.x)
            break;

        accumulateColumnWise(x, resolution, residual, result);
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    if(workGroupId != 1)
        return;

    result[0] = accumulatePartialResults(resolution.x, result);
}