#include "Math.hl"

__kernel void NormalizedCrossCorrelation(__global float* volume,
                                            int patchCoordX, int patchCoordY,
                                            int patchCoordZ, __local float* patch,
                                            __global float* nccVolume,
                                            int patchResolutionX,
                                            int patchResolutionY,
                                            int patchResolutionZ,
                                            float volumeMean)
{
    float convexParameter = 0.9F;

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
    
    float nccValue = 0.0F;
    float smoothnessValue = 0.0F;
    float smoothnessTmp = 0.0F;
    float candidatePatchMean = 0.0F;
    float normalizationFactor = 0.0F;

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
                    currentVolID = Flatten3D((uint3)(gid.x + x, gid.y + y, gid.z + z), res);
                    candidatePatchMean += volume[currentVolID];

                    inputPatchMean += patchVoxel;
                    numberOfNonNanValuesInPatch++;
                }
            }
        }
    }
    inputPatchMean /= (float)numberOfNonNanValuesInPatch;
    candidatePatchMean /= (float)numberOfNonNanValuesInPatch;

    float meanDifferencePatch = 0.0f;
    float meanDifferenceCandidate = 0.0f;
    float squaredMeanDifferencePatch = 0.0f;
    float squaredMeanDifferenceCandidate = 0.0f;

    if (gid.x > maximalStartCoordinate.x || gid.y > maximalStartCoordinate.y || gid.z > maximalStartCoordinate.z)
    {
        nccValue = NAN;
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
                        nccValue = NAN;
                        x = patchRes.x;
                        y = patchRes.y;
                        z = patchRes.z;
                    }
                    else {
                        currentPatchID = Flatten3D((uint3)(x, y, z), patchRes);
                        patchVoxel = patch[currentPatchID];

                        if (!isnan(patchVoxel))
                        {
                            meanDifferencePatch = patchVoxel - inputPatchMean;
                            meanDifferenceCandidate = volumeVoxel - candidatePatchMean;
                            nccValue += (meanDifferencePatch * meanDifferenceCandidate);
                            squaredMeanDifferencePatch += meanDifferencePatch * meanDifferencePatch;
                            squaredMeanDifferenceCandidate += meanDifferenceCandidate * meanDifferenceCandidate;
                        }

                        smoothnessTmp = inputPatchMean - volumeVoxel;
                        smoothnessValue += smoothnessTmp * smoothnessTmp;
                    }
                }
            }
        }
    }

    normalizationFactor = sqrt(squaredMeanDifferencePatch * squaredMeanDifferenceCandidate);

    if (normalizationFactor != 0)
    {
        nccValue /= normalizationFactor;
    }
    else
    {
        nccValue /= 0.0001;
    }

    if (isnan(nccValue))
    {
        nccVolume[Flatten3D(gid, res)] = nccValue;
    }
    else
    {
        nccVolume[Flatten3D(gid, res)] = (convexParameter * (-1) * nccValue) + (1 - convexParameter) * smoothnessValue;
//        nccVolume[Flatten3D(gid, res)] = -nccValue;
    }
}
