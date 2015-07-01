#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"
#include "Matrix4x4.hl"

__kernel void correct(MEMORY2D(projection),
                      __global float* vol,
                      MEMORY2D(rayLength),
                      __constant ProjectionGeometry* projectionGeometry,
                      __constant float* transform,
                      __constant VolumeGeometry* volumeGeometry,
                      float lambda,
                      unsigned int nSamples)
{
    const uint3 gid = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));
    if(any(gid >= volumeGeometry->subVolumeResolution))
        return;

    const uint voxelIndex = Flatten3D(gid, volumeGeometry->subVolumeResolution);

    float3 worldSpacePositionOfVoxelCenter = volumeGeometry->bBoxMin.xyz + (convert_float3(gid) + (float3)(0.5f, 0.5f, 0.5f)) * volumeGeometry->voxelSize.xyz;
    float3 projected = TransformCoordG(transform, worldSpacePositionOfVoxelCenter);

    const float2 coordinatesInProjection = projected.xy + 0.5f * convert_float2(MEMORY2D_SIZE(projection)); // WHY???

    float residualValue = nSamples == 1 ? READ_MEMORY2D(projection, coordinatesInProjection) : 0.0f;

    for(unsigned int i = 0; i < nSamples - 1; i++)
    {
        for(unsigned int j = 0; j < nSamples - 1; j++)
        {
            const float2 oversampledCoordinatesInProjection = coordinatesInProjection + (float2)(i, j);
            residualValue += READ_MEMORY2D(projection, oversampledCoordinatesInProjection);
        }
    }

    residualValue /= (nSamples * nSamples);

    float thisRayLength = READ_MEMORY2D(rayLength, coordinatesInProjection);

    // test for zero, because 0.0 is explicitly stored in forward projection
    if(!isnan(residualValue) && thisRayLength != 0)
    {		
        const float maxVoxelSize = max(max(volumeGeometry->voxelSize.x, volumeGeometry->voxelSize.y), volumeGeometry->voxelSize.z);
        const float correctionFactor = lambda / max(maxVoxelSize, thisRayLength);

        vol[voxelIndex] += residualValue * correctionFactor;

        #ifdef VOLUME_MINIMUM_INTENSITY
                vol[voxelIndex] = max(VOLUME_MINIMUM_INTENSITY, vol[voxelIndex]);
        #endif
        #ifdef VOLUME_MAXIMUM_INTENSITY
                vol[voxelIndex] = min(VOLUME_MAXIMUM_INTENSITY, vol[voxelIndex]);
        #endif
    }
}