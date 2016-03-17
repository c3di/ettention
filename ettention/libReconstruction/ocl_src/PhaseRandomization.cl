__kernel void computePhaseRandomizedImage(uint2 resolution, __global float* imageRandom, __global float* imageMagnitude, __global float* imagePhaseShifted_real, __global float* imagePhaseShifted_imaginary)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= resolution))
        return;

    const uint i = gid.y * resolution.x + gid.x;
    imagePhaseShifted_real[i]      = imageMagnitude[i] * cos(M_PI_F * (imageRandom[i] + 1.0f));
    imagePhaseShifted_imaginary[i] = imageMagnitude[i] * sin(M_PI_F * (imageRandom[i] + 1.0f));
}