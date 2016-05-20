#include "Math.hl"

__kernel void SmoothedSquaredDifferencesSum(__global float* volume,
                                            int patchCoordX, int patchCoordY,
                                            int patchCoordZ, __local float* patch,
                                            __global float* ssdVolume,
                                            int patchResolutionX,
                                            int patchResolutionY,
                                            int patchResolutionZ,
                                            float volumeMean)
{
    float convexParameter = 0.999F;

    const uint flatLocalId = get_local_id(2) * get_local_size(1) * get_local_size(0) + get_local_id(1) * get_local_size(0) + get_local_id(0);
    const uint flatLocalSize = get_local_size(0) * get_local_size(1) * get_local_size(2);
    const uint patchSize = patchResolutionX * patchResolutionY * patchResolutionZ;
    const uint valuesToCopyPerThread = (uint)ceil((float)patchSize / (float)flatLocalSize);
    const uint myStartIndex = flatLocalId * valuesToCopyPerThread;
    const uint myEndIndex = min(myStartIndex + valuesToCopyPerThread, patchSize);
    uint3 res = (uint3)(get_global_size(0), get_global_size(1), get_global_size(2));
    uint3 gid = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));

    for(uint i = myStartIndex; i < myEndIndex; ++i)
    {
        uint3 patchCoord;
        patchCoord.x = i % patchResolutionX; // (patchResolutionY * patchResolutionZ);
        patchCoord.y = (i / patchResolutionX) % patchResolutionY;
        patchCoord.z = i / (patchResolutionX * patchResolutionY);
        patchCoord += (uint3)(patchCoordX, patchCoordY, patchCoordZ);
        uint patchIndex = Flatten3D(patchCoord, res);

        if (patchCoord.x < res.x && patchCoord.y < res.y && patchCoord.z < res.z)
        {
            patch[i] = volume[patchIndex];
        }
        else
        {
            patch[i] = NAN;
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    
    float ssdValue = 0.0F;
    float smoothnessValue = 0.0F;
    float difference = 0.0F;
    float smoothnessTmp = 0.0F;

    float volumeVoxel = 0.0F;
    float patchVoxel = 0.0F;

    uint3 patchRes = (uint3)(patchResolutionX, patchResolutionY, patchResolutionZ);
    uint3 maximalStartCoordinate = res - patchRes;

    uint currentVolID = 0;
    uint currentPatchID = 0;

    // compute mean of source part of patch that should be inserted for smoothnessParameter
    float inputPatchMean = 0.0F;
    uint numberOfNonNanValuesInPatch = 0;

    for (uint x = 0; x < patchRes.x; x++)
    {
        for (uint y = 0; y < patchRes.y; y++)
        {
            for (uint z = 0; z < patchRes.z; z++)
            {
                currentPatchID = Flatten3D((uint3)(x, y, z), patchRes);
                patchVoxel = patch[currentPatchID];

                if (!isnan(patchVoxel))
                {
                    inputPatchMean += patchVoxel;
                    numberOfNonNanValuesInPatch++;
                }
            }
        }
    }
    inputPatchMean /= (float)numberOfNonNanValuesInPatch;

    if (gid.x > maximalStartCoordinate.x || gid.y > maximalStartCoordinate.y || gid.z > maximalStartCoordinate.z)
    {
        ssdValue = NAN;
    }
    else
    {
        for (uint x = 0; x < patchRes.x; x++)
        {
            for (uint y = 0; y < patchRes.y; y++)
            {
                for (uint z = 0; z < patchRes.z; z++)
                {
                    currentVolID = Flatten3D((uint3)(gid.x + x, gid.y + y, gid.z + z), res);
                    volumeVoxel = volume[currentVolID];

                    if (isnan(volumeVoxel))
                    {
                        ssdValue = NAN;
                        x = patchRes.x;
                        y = patchRes.y;
                        z = patchRes.z;
                    }
                    else {
                        currentPatchID = Flatten3D((uint3)(x, y, z), patchRes);
                        patchVoxel = patch[currentPatchID];

                        if (!isnan(patchVoxel))
                        {
                            difference = volumeVoxel - patchVoxel;
                            ssdValue += difference * difference;
                        }
                        else
                        {
                            smoothnessTmp = inputPatchMean - volumeVoxel;
                            smoothnessValue += smoothnessTmp * smoothnessTmp;
                        }
                    }
                }
            }
        }
    }

   float expSmoothness = 1 / exp(smoothnessValue);
   float expSSD = 1 / exp(ssdValue / 510);

    if (isnan(ssdValue))
    {
        ssdVolume[Flatten3D(gid, res)] = ssdValue;
    }
    else
    {
        ssdVolume[Flatten3D(gid, res)] = (-1) * ((convexParameter * expSSD) + ((1 - convexParameter) * expSmoothness));
        //ssdVolume[Flatten3D(gid, res)] = (convexParameter * ssdValue) + (1 - convexParameter) * smoothnessValue;
    }
}
