#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "STEMProjectionGeometry.hl"
#include "BilinearInterpolation.hl"
#include "Matrix4x4.hl"
#include "Math.hl"

__kernel void correct(int2 projectionResolution, __global float* projection, __global float* vol, __global float* ray_length_inside_volume, __constant STEMProjectionGeometry* projectionGeometry, __constant Matrix4x4* transform, __constant VolumeGeometry* volumeGeometry, float lambda)
{
    const uint3 gid = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));
    if(any(gid >= volumeGeometry->subVolumeResolution))
    {
        return;
    }
    const uint voxelIndex = Flatten3D(gid, volumeGeometry->subVolumeResolution);

    float3 voxelCenter = GetVoxelCenter(gid, volumeGeometry);
    float3 projected = TransformCoordG(transform, voxelCenter) - (float3)(0.5f, 0.5f, 0.0f);

    if(any(projected.xy < (float2)(0.0f, 0.0f)) || any(projected.xy > (float2)(projectionResolution.x - 2, projectionResolution.y - 2)))
    {
        return;
    }

    const float2 clampedCoordinatesInProjection = clamp(projected.xy, (float2)(0.0f, 0.0f), (float2)(projectionResolution.x - 2, projectionResolution.y - 2));

    const float residualValue = bilinearInterpolation(clampedCoordinatesInProjection, projectionResolution, projection);
    const float rayLength = bilinearInterpolation(clampedCoordinatesInProjection, projectionResolution, ray_length_inside_volume);

    if(!isnan(residualValue))
    {
        const float focalDifferenceBetweenImages = projectionGeometry->focalDifferenceBetweenImages;
        const float focalDistance = projectionGeometry->focalDistance;
        const float distanceFromImagePlane = projected.z;

        const float regularizationFactor = clamp(1.0f - fabs(focalDistance - distanceFromImagePlane) / focalDifferenceBetweenImages, 0.0f, 1.0f);
        vol[voxelIndex] += residualValue * lambda * regularizationFactor / rayLength;
    }
}