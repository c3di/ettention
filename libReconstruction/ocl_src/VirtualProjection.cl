#include "Math.hl"

__kernel void compensate(uint2 resolution, __global float* projection, __global float* travel_length, float constantLength)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= resolution))
        return;

    const uint i = gid.y * resolution.x + gid.x;
    projection[i] = travel_length[i] > EPSILON ? projection[i] * constantLength / travel_length[i] : 0.0f;
    travel_length[i] = constantLength;
}
