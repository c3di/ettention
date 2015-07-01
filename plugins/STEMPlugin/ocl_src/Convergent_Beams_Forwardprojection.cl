#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ConvergentBeams.hl"
#include "ImageHandler.hl"

#define INIT_RAY                float accumulatedValue = 0.0f;
#define ADDITIONAL_DDA_PARAMS   MEMORY3D(volume)
#define FILTER_OPERATOR         accumulatedValue += deltaT * READ_MEMORY3D(volume, volumeCoordinate);
#define RETURN_RESULT           return (float2) (accumulatedValue, distanceInsideBoundingBox);

#include "DDARaytracer.hl"

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      MEMORY3D(volume),
                      __global float* random,
                      __global float* volume_traversal_length,
                      __constant ProjectionGeometry *projectionGeometry,
                      __constant VolumeGeometry *volumeGeometry,
                      __constant ConvergentBeamsParameter *convergentBeamsParameter,
                      int nSamples)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));
    if(any(gid >= projectionResolution))
        return;

    float2 traversalResult = (float2)(0.0f, 0.0f);

    int nUnRejectedSamples = 0;
    bool sampleWasRejected;

    float4 normalizedBeamDirection = normalize(projectionGeometry->ray);

    const float4 beamSource = getBeamSource(projectionGeometry->source, gid.x, gid.y, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch);
    const float  focalDistance = convergentBeamsParameter->focalDistance;

    unsigned int i = gid.y * projectionResolution.x + gid.x;
    for(unsigned int sampleIndexX = 0; sampleIndexX < nSamples; sampleIndexX++)
    for(unsigned int sampleIndexY = 0; sampleIndexY < nSamples; sampleIndexY++)
    {

        const float4 raySource = getRaySourcePosition(gid.x, gid.y, nSamples, beamSource, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch, sampleIndexX, sampleIndexY, random, focalDistance, convergentBeamsParameter->openingHalfAngleTangens, &sampleWasRejected);

        if(sampleWasRejected)
            continue;

        const float4 rayDirection = getRayDirection(gid.x, gid.y, raySource, beamSource, normalizedBeamDirection, focalDistance);

        traversalResult += ddaTraverseRay( raySource, rayDirection, volumeGeometry, MEMORY3D_PARAMS(volume) );
        nUnRejectedSamples++;
    }
    traversalResult /= (float)nUnRejectedSamples;

    projection[i] += traversalResult.x;
    volume_traversal_length[i] += traversalResult.y;
}