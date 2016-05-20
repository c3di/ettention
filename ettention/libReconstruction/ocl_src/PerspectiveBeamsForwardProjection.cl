#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"
#include "RayUtils.hl"

#define INIT_RAY                float accumulatedValue = 0.0f; float accumulatedRayLength = 0.0f;
#define ADDITIONAL_DDA_PARAMS   TYPE_3D_READONLY(volume),TYPE_3D_MASK(mask)
#define FILTER_OPERATOR         accumulatedValue += deltaT * READ_3D_READONLY(volume, volumeCoordinate); accumulatedRayLength += deltaT * READ_3D_MASK_IF_POSSIBLE(mask, volumeCoordinate);
#define RETURN_RESULT           return (float2)(accumulatedValue, accumulatedRayLength);

#include "DDARaytracer.hl"

float3 getSamplePos(float3 pixelBase, float3 horizontalPitch, float3 verticalPitch, uint sampleIndex, uint samplesPerDimension)
{
    uint dh = sampleIndex % samplesPerDimension;
    uint dv = sampleIndex / samplesPerDimension;
    return pixelBase + ((float)dh + 0.5f) / (float)samplesPerDimension * horizontalPitch + ((float)dv + 0.5f) / (float)samplesPerDimension * verticalPitch;
}

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      TYPE_3D_READONLY(volume),
                      TYPE_3D_MASK(mask),
                      __global float* volume_traversal_length,
                      __constant ProjectionGeometry* projectionGeometry,
                      __constant VolumeGeometry* volumeGeometry,
                      unsigned int samples)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));

    if(any(gid >= projectionResolution))
        return;

    float accum_result = 0;
    float accum_raylength = 0;
    uint total_samples = samples * samples;

    for(uint sampleIndex = 0; sampleIndex < total_samples; ++sampleIndex)
    {
        const float4 pixelBase = projectionGeometry->detector + (float)gid.x * projectionGeometry->horizontalPitch + (float)gid.y * projectionGeometry->verticalPitch;
        const float3 detectorPos = getSamplePos(pixelBase.xyz, projectionGeometry->horizontalPitch.xyz, projectionGeometry->verticalPitch.xyz, sampleIndex, samples);
        const float4 rayDir = (float4)(normalize(detectorPos - projectionGeometry->source.xyz), 0.0f);

        float2 traversalResult = ddaTraverseRay(projectionGeometry->source, rayDir, volumeGeometry, MEMORY_PARAMS(volume), MEMORY_PARAMS(mask));

        accum_result += traversalResult.x;
        accum_raylength += traversalResult.y;
    }
    const uint i = gid.y * projectionResolution.x + gid.x;
    projection[i] += accum_result / total_samples;
    volume_traversal_length[i] += accum_raylength / total_samples;
}