#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "RayUtils.hl"
#include "Matrix4x4.hl"
#include "ImageHandler.hl"
#include "BlobRaytracer.hl"

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      __global float* volume_traversal_length,
                      MEMORY3D(volume),
                      __constant ProjectionGeometry *projectionGeometry,
                      __constant VolumeGeometry *volumeGeometry,
                      __constant float* projection2world,
                      // blobs
                      __global float* globalBlobSampler,
                      __local float* blobSampler,
                      uint blobSampleCount,
                      float blobRadius,
                      unsigned int samples)
{
    // copy blobSampler to local memory
    const uint flatLocalId = get_local_id(1) * get_local_size(0) + get_local_id(0);
    const uint flatLocalSize = get_local_size(0) * get_local_size(1);
    const uint valuesToCopyPerThread = (uint)ceil((float)blobSampleCount / (float)flatLocalSize);
    const uint myStartIndex = flatLocalId * valuesToCopyPerThread;
    const uint myEndIndex = min(myStartIndex + valuesToCopyPerThread, blobSampleCount);
    for(uint i = myStartIndex; i < myEndIndex; ++i)
    {
        blobSampler[i] = globalBlobSampler[i];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    blobRadius *= volumeGeometry->voxelSize.x;
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);
    Matrix4x4 privateMatrix;
    matrix4x4_create_from_array(&privateMatrix, projection2world);
    float4 worldPos = multiply(&privateMatrix, (float4)((float)x, (float)y, 1.0f, 1.0f));
    worldPos = (float4)(worldPos.xyz / worldPos.w, 0.0f);

    float2 result = BlobTraverseRay(MEMORY3D_PARAMS(volume),
                                    0,
                                    blobSampler,
                                    blobSampleCount,
                                    projectionGeometry->source,
                                    (float4)(normalize(worldPos.xyz - projectionGeometry->source.xyz), 0.0f),
                                    volumeGeometry->bBoxMin,
                                    volumeGeometry->bBoxMax,
                                    volumeGeometry->bBoxMinComplete,
                                    volumeGeometry->bBoxMaxComplete,
                                    blobRadius,
                                    volumeGeometry->voxelSize,
                                    (int4)(convert_int3(volumeGeometry->subVolumeResolution), 0));

    const uint i = y * projectionResolution.x + x;
    projection[i] += result.x;
    volume_traversal_length[i] += result.y;
}