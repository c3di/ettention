__kernel void computePhase(uint2 resolution, __global float* imageReal, __global float* imageImaginary, __global float* resultPhase)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= resolution))
        return;

    const uint i = gid.y * resolution.x + gid.x;
    resultPhase[i] = atan2(imageImaginary[i], imageReal[i]);
}