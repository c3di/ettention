__kernel void multiply(uint2 resolution, __global float* imageAReal, __global float* imageAImaginary, __global float* imageBReal, __global float* imageBImaginary, __global float* resultReal, __global float* resultImaginary)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= resolution))
        return;

    const uint i = gid.y * resolution.x + gid.x;
    resultReal[i] = imageAReal[i] * imageBReal[i] - imageAImaginary[i] * imageBImaginary[i];
    resultImaginary[i] = imageAReal[i] * imageBImaginary[i] + imageAImaginary[i] * imageBReal[i];
}