#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"
#include "RayUtils.hl"

#define INIT_RAY                float accumulatedValue = 0.0f;
#define ADDITIONAL_DDA_PARAMS   MEMORY3D(volume)
#define FILTER_OPERATOR         accumulatedValue += deltaT * READ_MEMORY3D(volume, volumeCoordinate);
#define RETURN_RESULT           return (float2)(accumulatedValue, distanceInsideBoundingBox);

#include "DDARaytracer.hl"

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      MEMORY3D(volume),
                      __global float* volume_traversal_length,
                      __constant ProjectionGeometry* projectionGeometry,
                      __constant VolumeGeometry* volumeGeometry,
                      unsigned int samples)
{
    const uint2 gid = (uint2)(get_global_id(0), get_global_id(1));

    //if(gid.y == 492 && gid.x == 305) return;

    if(any(gid >= projectionResolution))
        return;

    const float4 source = projectionGeometry->source;

    uint2 sampleIndex;
    float accum_result = 0;
    float accum_raylength = 0;
    float total_samples = samples * samples;
    for(sampleIndex.x = 0; sampleIndex.x < samples; sampleIndex.x++)
    {
        for(sampleIndex.y = 0; sampleIndex.y < samples; sampleIndex.y++)
        {
            const float4 detector = getRayPosition(projectionGeometry->detector, gid.x, gid.y, projectionGeometry->horizontalPitch, projectionGeometry->verticalPitch, sampleIndex, samples);
            const float4 ray = normalize(detector - source);

            float2 traversalResult = ddaTraverseRay(source, ray, volumeGeometry, MEMORY3D_PARAMS(volume) );

            accum_result += traversalResult.x;
            accum_raylength += traversalResult.y;
        }
    }
    unsigned int i = gid.y * projectionResolution.x + gid.x;
    projection[i] += accum_result / total_samples;
    volume_traversal_length[i] += accum_raylength / total_samples;
}