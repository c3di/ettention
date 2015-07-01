#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"

#define INIT_RAY                float accumulatedValue = 0.0f;
#define ADDITIONAL_DDA_PARAMS   MEMORY3D(volume)
#define FILTER_OPERATOR         accumulatedValue += deltaT * READ_MEMORY3D(volume, volumeCoordinate);
#define RETURN_RESULT           return (float2) (accumulatedValue, distanceInsideBoundingBox);

#include "DDARaytracer.hl"

float4 getRaySourcePosition(float4 beamSource, unsigned int x, unsigned int y, float4 pitchY, float4 pitchZ)
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

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      MEMORY3D(volume),
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
        
    const float4 source = getRaySourcePosition(projectionGeometry->source, gid.x, gid.y, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch);
    const float4 ray = getRayDirection(projectionGeometry->ray);
    for (sampleIndex.x = 0; sampleIndex.x < samples; sampleIndex.x++)
    {
        for (sampleIndex.y = 0; sampleIndex.y < samples; sampleIndex.y++)
        {
            float2 traversalResult = ddaTraverseRay( source, ray, volumeGeometry, MEMORY3D_PARAMS(volume) );
            
            accum_result += traversalResult.x;
            accum_raylength += traversalResult.y;
        }
    }
    
    unsigned int i = gid.y * projectionResolution.x + gid.x;
    projection[i] += accum_result / total_samples;
    volume_traversal_length[i] += accum_raylength / total_samples;
}