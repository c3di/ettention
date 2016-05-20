#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "STEMProjectionGeometry.hl"
#include "BilinearInterpolation.hl"
#include "Matrix4x4.hl"
#include "Math.hl"

float getRequiredBlurrRadiusForDistanceFromFocalPlane(float distanceFromFocalPlane, __constant STEMProjectionGeometry *projectionGeometry)
{
    return distanceFromFocalPlane * projectionGeometry->openingHalfAngleTangens;
}

float lookUpPrefilteredResidualValue(float2 clampedCoordinatesInProjection, int2 projectionSize, __global float* residual, float distanceFromFocalPlane, __constant STEMProjectionGeometry *projectionGeometry, unsigned int numberOfPrecomputedResiduals)
{
    float radius = getRequiredBlurrRadiusForDistanceFromFocalPlane(distanceFromFocalPlane, projectionGeometry);
    radius = fmax(0.5f, radius);

    const float radiusInLogSpace = log2(radius) + 1.0f;

    float radius0f;
    const float i = fract(radiusInLogSpace, &radius0f);

    const int radius0 = (int)radius0f;
    const int radius1 = radius0 + 1;

    const int imageSize = projectionSize.x * projectionSize.y;
    const int offset0 = imageSize * radius0;
    const int offset1 = imageSize * radius1;

    if(offset1 > (numberOfPrecomputedResiduals - 1) * imageSize)
        return 0.0f;

    float value0 = bilinearInterpolation(clampedCoordinatesInProjection, projectionSize, &residual[offset0]);
    float value1 = bilinearInterpolation(clampedCoordinatesInProjection, projectionSize, &residual[offset1]);

    return i * value1 + (1.0f - i) * value0;
}

__kernel void correct(int2 projectionResolution, __global float* projection, __global float* vol, __global float* ray_length_inside_volume, __constant STEMProjectionGeometry *projectionGeometry, __constant Matrix4x4* transform, __constant VolumeGeometry* volumeGeometry, float lambda, unsigned int numberOfPrecomputedResiduals)
{
    const uint3 gid = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));

    if(any(gid >= volumeGeometry->subVolumeResolution))
    {
        return;
    }
    const uint voxelIndex = Flatten3D(gid, volumeGeometry->subVolumeResolution);

    // position 
    float3 voxelCenter = GetVoxelCenter(gid, volumeGeometry);
    float3 projected = TransformCoordG(transform, voxelCenter);

    const float2 clampedCoordinatesInProjection = clamp(projected.xy - (float2)(0.5f, 0.5f), (float2)(0.0f, 0.0f), (float2)(projectionResolution.x - 2, projectionResolution.x - 2));

    const float focalDistance = projectionGeometry->focalDistance;
    const float distanceFromImagePlane = projected.z;
    const float distanceFromFocalPlane = fabs(focalDistance - distanceFromImagePlane);

    const float residualValue = lookUpPrefilteredResidualValue(clampedCoordinatesInProjection, projectionResolution, projection, distanceFromFocalPlane, projectionGeometry, numberOfPrecomputedResiduals);
    const float rayLength = bilinearInterpolation(clampedCoordinatesInProjection, projectionResolution, ray_length_inside_volume);

    const float correctionFactor = residualValue * lambda / rayLength;

    if(!isnan(correctionFactor))
    {
        vol[voxelIndex] += correctionFactor;
    }
}