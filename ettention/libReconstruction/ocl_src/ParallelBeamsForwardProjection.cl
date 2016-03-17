#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"

#define INIT_RAY                float accumulatedValue = 0.0f; float accumulatedRayLength = 0.0f;
#define ADDITIONAL_DDA_PARAMS   TYPE_3D_READONLY(volume),TYPE_3D_MASK(mask)
#define FILTER_OPERATOR         accumulatedValue += deltaT * READ_3D_READONLY(volume, volumeCoordinate); accumulatedRayLength += deltaT * READ_3D_MASK_IF_POSSIBLE(mask, volumeCoordinate);
#define RETURN_RESULT           return (float2)(accumulatedValue, accumulatedRayLength);

#include "DDARaytracer.hl"

float4 getRaySourcePosition(float4 beamSource, unsigned int x, unsigned int y, float4 pitchY, float4 pitchZ, uint2 samplingPosition, uint samplesInRow)
{
    const float sampleDistance = 1.0f / (samplesInRow);
    const float offset = sampleDistance / 2.0f;

    const float fx = (float)x + offset + sampleDistance * samplingPosition.x;
    const float fy = (float)y + offset + sampleDistance * samplingPosition.y;

    const float4 relativePositionU = pitchY * fx;
    const float4 relativePositionV = pitchZ * fy;

    return beamSource + relativePositionU + relativePositionV;
}

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      TYPE_3D_READONLY(volume),
                      TYPE_3D_MASK(mask),
                      __global float* volume_traversal_length,
                      __constant ProjectionGeometry *projectionGeometry,
                      __constant VolumeGeometry *volumeGeometry,
                      unsigned int samples)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));

    uint2 sampleIndex;
    float accum_result = 0.0f;
    float accum_raylength = 0.0f;
    float total_samples = (float)(samples * samples);

    const float4 ray = projectionGeometry->ray;
    for (sampleIndex.x = 0; sampleIndex.x < samples; sampleIndex.x++)
    {
        for (sampleIndex.y = 0; sampleIndex.y < samples; sampleIndex.y++)
        {
            const float4 source = getRaySourcePosition(projectionGeometry->source, gid.x, gid.y, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch, sampleIndex, samples);
            float2 traversalResult = ddaTraverseRay( source, ray, volumeGeometry, MEMORY_PARAMS(volume), MEMORY_PARAMS(mask));

            accum_result += traversalResult.x;
            accum_raylength += traversalResult.y;
        }
    }
    
    unsigned int index = gid.y * projectionResolution.x + gid.x;
    projection[index] += accum_result / total_samples;
    volume_traversal_length[index] += accum_raylength / total_samples;
}