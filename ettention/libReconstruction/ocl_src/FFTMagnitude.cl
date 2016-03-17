__kernel void computeMagnitude(uint2 resolution, __global float* imageReal, __global float* imageImaginary, __global float* resultMagnitude)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= resolution))
        return;

    const uint i = gid.y * resolution.x + gid.x;
    resultMagnitude[i] = sqrt(imageImaginary[i] * imageImaginary[i] + imageReal[i] * imageReal[i]);
}