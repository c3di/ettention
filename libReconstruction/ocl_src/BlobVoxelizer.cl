#include "HalfFloatSupport.hl"

void __kernel voxelize(FLOAT_TYPE blobVolume,
                       FLOAT_TYPE voxelVolume,
                       __global float* blobVoxelizerSampler,
                       uint4 blobVolumeRes,
                       int blobRadius)
{ 
    const int4 id = (int4)((int)get_global_id(0), (int)get_global_id(1), (int)get_global_id(2), 0);
    const uint4 voxelVolumeRes = (uint4)(get_global_size(0), get_global_size(1), get_global_size(2), 1);
    const uint samplerRes = 2 * blobRadius + 1;
    
    float val = 0.0f;
    for(int dz = -blobRadius; dz <= blobRadius; ++dz)
    {
        for(int dy = -blobRadius; dy <= blobRadius; ++dy)
        {
            for(int dx = -blobRadius; dx <= blobRadius; ++dx)
            {
                const int4 delta = (int4)(dx, dy, dz, 0);
                const int4 blob = id + blobRadius + delta;
                const float blobValue = READ_FLOAT(blobVolume, blob.z * blobVolumeRes.y * blobVolumeRes.x + blob.y * blobVolumeRes.x + blob.x);
                const int4 samplerIndex = (int4)(dx + blobRadius, dy + blobRadius, dz + blobRadius, 0);
                const float weight = blobVoxelizerSampler[samplerIndex.z * samplerRes * samplerRes + samplerIndex.y * samplerRes + samplerIndex.x];
                val += weight * blobValue;
            }
        }
    }
    STORE_FLOAT(voxelVolume, id.z * voxelVolumeRes.y * voxelVolumeRes.x + id.y * voxelVolumeRes.x + id.x, val);
}