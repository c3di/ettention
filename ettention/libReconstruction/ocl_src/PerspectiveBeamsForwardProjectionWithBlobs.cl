#include "HalfFloatSupport.hl"
#include "ImageHandler.hl"

#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "Matrix4x4.hl"
#include "BlobRaytracer.hl"

__kernel void execute(uint2 projectionResolution,
                      __global float* projection,
                      __global float* volume_traversal_length,
                      TYPE_3D_READONLY(volume),
                      TYPE_3D_MASK(mask), // USE IT LATER !!!
                      __constant ProjectionGeometry *projectionGeometry,
                      __constant VolumeGeometry *volumeGeometry,
                      __constant Matrix4x4* projection2world,
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

    // forward projection
    blobRadius *= volumeGeometry->voxelSize.x;
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);
    float3 worldPos = TransformCoordG(projection2world, (float3)((float)x, (float)y, 1.0f));

    float2 result = BlobTraverseRay(MEMORY_PARAMS(volume),
                                    0, // ?????
                                    blobSampler,
                                    blobSampleCount,
                                    projectionGeometry->source,
                                    (float4)(normalize(worldPos - projectionGeometry->source.xyz), 0.0f),
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