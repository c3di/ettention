__kernel void multiply (uint2 resolution, __global float* realProjection, __global float* virtualProjection,  __global float* residual)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= resolution))
        return;

    const uint i = gid.y * resolution.x + gid.x;
    residual[i] = realProjection[i] * virtualProjection[i];
}