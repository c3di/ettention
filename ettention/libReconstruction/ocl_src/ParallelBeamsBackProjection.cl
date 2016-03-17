#include "HalfFloatSupport.hl"
#include "ImageHandler.hl"
#include "Math.hl"

#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "Matrix4x4.hl"

__kernel void correct(TYPE_2D_READONLY(projection),
                      TYPE_3D_READWRITE(vol),
                      TYPE_3D_MASK(mask),
                      TYPE_2D_READONLY(rayLength),
                      __constant ProjectionGeometry* projectionGeometry,
                      __constant Matrix4x4* transform,
                      __constant VolumeGeometry* volumeGeometry,
                      float lambda,
                      const unsigned int nSamples,
                      __constant float4* samples )
{
    const uint3 gid = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));
    if(any(gid >= volumeGeometry->subVolumeResolution))
        return;

    const uint voxelIndex = Flatten3D(gid, volumeGeometry->subVolumeResolution);

    float projectedValue = 0.0f;
    unsigned int usedSamples = 0;
    for(unsigned int sampleIndex = 0; sampleIndex < nSamples; sampleIndex++)
    {
        float3 positionOffset = samples[sampleIndex].xyz;
        float rayDistanceInsideVoxel = samples[sampleIndex].w;

        float3 samplingPos = GetVoxelCenter(gid, volumeGeometry) + positionOffset;
        float2 projected = TransformCoordG(transform, samplingPos).xy;

        float thisRayLength = READ_2D_READONLY(rayLength, projected);
        float residualValue = READ_2D_READONLY(projection, projected);

        const float sampleWeight = rayDistanceInsideVoxel / thisRayLength;
        const float sampleContribution = residualValue * lambda * sampleWeight;

        projectedValue += (thisRayLength > EPSILON) ? sampleContribution : 0.0f;
        usedSamples += (thisRayLength > EPSILON) ? 1 : 0;
    }

    float volumeValue = READ_3D_READWRITE(vol, voxelIndex);
    if(usedSamples != 0)
    {
        float maskValue = READ_3D_MASK_IF_POSSIBLE(mask, convert_float4((uint4)(gid, 0)));
        volumeValue += maskValue * projectedValue / (float)usedSamples;
    }
    WRITE_3D_READWRITE(vol, voxelIndex, volumeValue);
}