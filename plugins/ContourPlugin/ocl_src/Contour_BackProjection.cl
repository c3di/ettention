#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"
#include "Matrix4x4.hl"
#include "HalfFloatSupport.hl"
#include "Math.hl"

__kernel void back(MEMORY2D(projection),
                   FLOAT_TYPE vol,
                   FLOAT_TYPE mask,
                   MEMORY2D(rayLength),
                   __constant ProjectionGeometry* projectionGeometry,
                   __constant Matrix4x4* transform,
                   __constant VolumeGeometry* volumeGeometry,
                   float lambda)
{
    const uint3 gid = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));

    const uint voxelIndex = Flatten3D(gid, volumeGeometry->subVolumeResolution);

    float3 voxelCenter = GetVoxelCenter(gid, volumeGeometry);
    float2 projected = TransformCoordG(transform, voxelCenter).xy;

    float thisRayLength = READ_MEMORY2D(rayLength, projected);
    float residualValue = READ_MEMORY2D(projection, projected);

    float projectedValue = (thisRayLength > EPSILON) ? residualValue * lambda / thisRayLength : 0.0f;

    float volumeValue = READ_FLOAT(vol,  voxelIndex);
    float maskValue   = READ_FLOAT(mask, voxelIndex);

    volumeValue += maskValue != 0.0f ? projectedValue : 0.0f;
    STORE_FLOAT(vol, voxelIndex, volumeValue);
}