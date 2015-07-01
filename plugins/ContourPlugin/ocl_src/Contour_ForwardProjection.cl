#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"

#define INIT_RAY                float accumulatedValue = 0.0f; float accumulatedRayLength = 0.0f;
#define ADDITIONAL_DDA_PARAMS   MEMORY3D(volume),MEMORY3D(mask)
#define FILTER_OPERATOR         accumulatedValue += deltaT * READ_MEMORY3D(volume, volumeCoordinate); accumulatedRayLength += READ_MEMORY3D(mask, volumeCoordinate) != 0.0f ? deltaT : 0.0f;
#define RETURN_RESULT           return (float2)( accumulatedValue, accumulatedRayLength );

#include "DDARaytracer.hl"

float4 getRaySourcePosition(float4 beamSource, unsigned int x, unsigned int y, float4 pitchY, float4 pitchZ)
{
    const float fx = (float)x + 0.5f;
    const float fy = (float)y + 0.5f;

    const float4 relativePositionU = pitchY * fx;
    const float4 relativePositionV = pitchZ * fy;

    return beamSource + relativePositionU + relativePositionV;
}

__kernel void forward(uint2 projectionResolution,
                      __global float* projection,
                      MEMORY3D(volume),
                      MEMORY3D(mask),
                      __global float* volume_traversal_length,
                      __constant ProjectionGeometry *projectionGeometry,
                      __constant VolumeGeometry *volumeGeometry)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));

    const float4 source = getRaySourcePosition(projectionGeometry->source, gid.x, gid.y, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch);
    const float2 traversalResult = ddaTraverseRay(source, projectionGeometry->ray, volumeGeometry, MEMORY3D_PARAMS(volume), MEMORY3D_PARAMS(mask));

    uint imageIndex = gid.y * projectionResolution.x + gid.x;
    projection[imageIndex] = traversalResult.x;
    volume_traversal_length[imageIndex] = traversalResult.y;
}