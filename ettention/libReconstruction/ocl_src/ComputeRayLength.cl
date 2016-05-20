#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "RayUtils.hl"

float4 getRaySourcePosition(float4 beamSource, uint x, unsigned int y, float4 pitchY, float4 pitchZ)
{
    const float fx = ((float)x) + 0.5f;
    const float fy = ((float)y) + 0.5f;

    const float4 relativePositionU = pitchY * fx;
    const float4 relativePositionV = pitchZ * fy;

    return beamSource + relativePositionU + relativePositionV;
}

float4 getRayDirection(float4 beamSource)
{
    return beamSource;
}

__kernel void execute(uint2 resolution, __global float* ray_length, __global ProjectionGeometry* projectionGeometry, __global VolumeGeometry* volumeGeometry)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= resolution))
        return;

    const float4 source = getRaySourcePosition(projectionGeometry->source, gid.x, gid.y, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch);
    const float4 ray = getRayDirection(projectionGeometry->ray);
    const float4 invRay = invertVectorSafely(ray);
    const float2 tLimits = getBoundingBoxEntryAndExitPointInRayCoordinates(volumeGeometry->bBoxMinComplete, volumeGeometry->bBoxMaxComplete, source, invRay);

    const uint i = gid.y * resolution.x + gid.x;
    ray_length[i] = fmax(tLimits.y - tLimits.x, 0.0f);
}