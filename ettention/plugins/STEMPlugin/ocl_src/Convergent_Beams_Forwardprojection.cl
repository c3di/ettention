#include "VolumeGeometry.hl"
#include "ImageHandler.hl"

#include "ConvergentBeams.hl"
#include "STEMProjectionGeometry.hl"

#define INIT_RAY                float accumulatedValue = 0.0f; float accumulatedRayLength = 0.0f;
#define ADDITIONAL_DDA_PARAMS   TYPE_3D_READONLY(volume),TYPE_3D_MASK(mask)
#define FILTER_OPERATOR         accumulatedValue += deltaT * READ_3D_READONLY(volume, volumeCoordinate); accumulatedRayLength += deltaT * READ_3D_MASK_IF_POSSIBLE(mask, volumeCoordinate);
#define RETURN_RESULT           return (float2)(accumulatedValue, accumulatedRayLength);

#include "DDARaytracer.hl"

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      TYPE_3D_READONLY(volume),
                      TYPE_3D_MASK(mask),
                      __global float* random,
                      __global float* volume_traversal_length,
                      __constant STEMProjectionGeometry *projectionGeometry,
                      __constant VolumeGeometry *volumeGeometry,
                      int samples)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= projectionResolution))
        return;

    float2 traversalResult = (float2)(0.0f, 0.0f);

    int nUnRejectedSamples = 0;
    bool sampleWasRejected;

    float4 normalizedBeamDirection = normalize(projectionGeometry->ray);

    const float4 beamSource = getBeamSource(projectionGeometry->source, gid.x, gid.y, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch);

    const float sourceDistanceFromTiltAxis = projectionGeometry->source.x * projectionGeometry->ray.x 
										   + projectionGeometry->source.y * projectionGeometry->ray.y
										   + projectionGeometry->source.z * projectionGeometry->ray.z;
    const float focalDistance = projectionGeometry->focalDistance - sourceDistanceFromTiltAxis;

    unsigned int i = gid.y * projectionResolution.x + gid.x;
    for(unsigned int sampleIndexX = 0; sampleIndexX < samples; sampleIndexX++)
    for(unsigned int sampleIndexY = 0; sampleIndexY < samples; sampleIndexY++)
    {

        const float4 raySource = getRaySourcePosition(gid.x, gid.y, samples, beamSource, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch, sampleIndexX, sampleIndexY, random, focalDistance, projectionGeometry->openingHalfAngleTangens, &sampleWasRejected);

        if(sampleWasRejected)
            continue;

        const float4 rayDirection = getRayDirection(gid.x, gid.y, raySource, beamSource, normalizedBeamDirection, focalDistance);

        traversalResult += ddaTraverseRay( raySource, rayDirection, volumeGeometry, MEMORY_PARAMS(volume), MEMORY_PARAMS(mask));
        nUnRejectedSamples++;
    }
    traversalResult /= (float)nUnRejectedSamples;

    projection[i] += traversalResult.x;
    volume_traversal_length[i] += traversalResult.y;
}