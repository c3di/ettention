#include "HalfFloatSupport.hl"
#include "ImageHandler.hl"
#include "Math.hl"

#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "Matrix4x4.hl"
#include "RayUtils.hl"

float getRayLengthInVoxel(float3 rayOrigin, float3 rayDirection, float3 voxelCenter, float3 voxelSize)
{
    float4 voxelBoundingBoxMin = (float4)(voxelCenter - 0.5f * voxelSize, 0.0f);
    float4 voxelBoundingBoxMax = (float4)(voxelCenter + 0.5f * voxelSize, 0.0f);
    float4 invRayDir = (float4)(1.0f / normalize(rayDirection), 0.0f);
    float2 tVec = getBoundingBoxEntryAndExitPointInRayCoordinates(voxelBoundingBoxMin, voxelBoundingBoxMax, (float4)(rayOrigin, 0.0f), invRayDir);
    return max(0.0f, tVec.y - tVec.x);
}

float3 getSamplePos(float3 voxelCenter, float3 voxelSize, uint sampleIndex, uint samplesPerDimension)
{
    float3 voxelBase = voxelCenter - 0.5f * voxelSize;
    uint dx = sampleIndex % samplesPerDimension;
    uint dy = (sampleIndex / samplesPerDimension) % samplesPerDimension;
    uint dz = sampleIndex / (samplesPerDimension * samplesPerDimension);
    return voxelBase + voxelSize * (float3)((float)dx + 0.5f, (float)dy + 0.5f, (float)dz + 0.5f) / (float)samplesPerDimension;
}

__kernel void correct(TYPE_2D_READONLY(projection),
                      TYPE_3D_READWRITE(vol),
                      TYPE_3D_MASK(mask),
                      TYPE_2D_READONLY(rayLength),
                      __constant ProjectionGeometry* projectionGeometry,
                      __constant Matrix4x4* transform,
                      __constant VolumeGeometry* volumeGeometry,
                      float lambda,
                      unsigned int nSamples)
{
    const uint3 gid = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));
    if(any(gid >= volumeGeometry->subVolumeResolution))
        return;

    float value = 0.0f;
    const float3 voxelCenter = GetVoxelCenter(gid, volumeGeometry);
    const uint sampleCount = nSamples * nSamples * nSamples;
    for(uint sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
    {
        const float3 samplePos = getSamplePos(voxelCenter, volumeGeometry->voxelSize.xyz, sampleIndex, nSamples);
        const float2 projectionCoordinates = TransformCoordG(transform, samplePos).xy;
        float rayLengthInVolume = READ_2D_READONLY(rayLength, projectionCoordinates);
        if(rayLengthInVolume >= EPSILON)
        {
            float rayLengthInVoxel = getRayLengthInVoxel(samplePos, samplePos - projectionGeometry->source.xyz, voxelCenter, volumeGeometry->voxelSize.xyz);
            float residualValue = READ_2D_READONLY(projection, projectionCoordinates);
            value += residualValue * rayLengthInVoxel / rayLengthInVolume;
        }
    }

    const uint voxelIndex = Flatten3D(gid, volumeGeometry->subVolumeResolution);
    float voxelValue = READ_3D_READWRITE(vol, voxelIndex);
    float maskValue = READ_3D_MASK_IF_POSSIBLE(mask, convert_float4((uint4)(gid, 0)));
    voxelValue += lambda * value * maskValue / (float)sampleCount;

#ifdef VOLUME_MINIMUM_INTENSITY
    voxelValue = max(VOLUME_MINIMUM_INTENSITY, voxelValue);
#endif
#ifdef VOLUME_MAXIMUM_INTENSITY
    voxelValue = min(VOLUME_MAXIMUM_INTENSITY, voxelValue);
#endif

    WRITE_3D_READWRITE(vol, voxelIndex, voxelValue);
}