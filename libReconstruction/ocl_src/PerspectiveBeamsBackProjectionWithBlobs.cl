#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "ImageHandler.hl"
#include "Matrix4x4.hl"
#include "BlobRaytracer.hl"
#include "HalfFloatSupport.hl"
#include "Math.hl"

float GetContributionFromPixel(int2 pixel, 
                               MEMORY2D(projection),
                               MEMORY2D(rayLength),
                               Matrix4x4* projection2world,
                               float4 emitterSource,
                               float4 blobCenter,
                               __global float* blobSampler,
                               uint blobSampleCount,
                               float blobRadiusSquared,
                               float* accumulatedWeights)
{
    if(all(0 <= pixel) && all(pixel < MEMORY2D_SIZE(projection)))
    {
        const float len = READ_MEMORY2D(rayLength, pixel);
        const float residual = READ_MEMORY2D(projection, pixel);
        if(len > 0.0f && residual != 0.0f)
        {
            float4 pixelCenter = multiply(projection2world, (float4)(convert_float2(pixel), 1.0f, 1.0f));
            float4 pixelRayDir = (float4)(normalize(pixelCenter.xyz / pixelCenter.w - emitterSource.xyz), 0.0f);
            float4 sourceToBlobCenter = (float4)(blobCenter.xyz - emitterSource.xyz, 0.0f);
            float t0 = dot(pixelRayDir, sourceToBlobCenter);
            float pixelRayToBlobCenterDistSquared = dot(sourceToBlobCenter, sourceToBlobCenter) - t0 * t0;
            if(blobRadiusSquared > pixelRayToBlobCenterDistSquared)
            {
                float blobTraversalLength = 2.0f * sqrt(blobRadiusSquared - pixelRayToBlobCenterDistSquared);
                float blobTraversalValue = SampleBlobFromGlobal(blobSampler, blobSampleCount, sqrt(pixelRayToBlobCenterDistSquared / blobRadiusSquared));
                *accumulatedWeights += blobTraversalValue;
                return len > EPSILON ? (residual * blobTraversalValue) / len : 0.0f;
            }
        }
    }
    return 0.0f;
}

__kernel void execute(MEMORY2D(projection),
                      FLOAT_TYPE vol,
                      MEMORY2D(rayLength),
                      __constant ProjectionGeometry* projectionGeometry,
                      __constant VolumeGeometry* volumeGeometry,
                      float lambda,
                      __constant float* world2projection,
                      __constant float* projection2world,
                      unsigned int nSamples,
                      __global float* blobSampler,
                      uint blobSampleCount,
                      float blobRadius)
{ 
    blobRadius *= volumeGeometry->voxelSize.x;
    const uint resX = get_global_size(0);
    const uint resY = get_global_size(1);
    const uint resZ = get_global_size(2);
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);
    const uint z = get_global_id(2);
    const uint id = z * resY * resX + y * resX + x;
    //vol[id] = 0.0f; // debug!
    Matrix4x4 privateWorld2projection;
    matrix4x4_create_from_array(&privateWorld2projection, world2projection);
    Matrix4x4 privateProjection2world;
    matrix4x4_create_from_array(&privateProjection2world, projection2world);

    const float4 blobCenter = (float4)((volumeGeometry->bBoxMin + (float4)((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f, 0.0f) * volumeGeometry->voxelSize).xyz, 1.0f);
    float4 projectedBlobCenter = multiply(&privateWorld2projection, blobCenter);
    float2 blobCenterParams = projectedBlobCenter.xy / projectedBlobCenter.w;

    if(projectedBlobCenter.w > 0.0f)
    {
        float4 p = (float4)((projectionGeometry->detector + blobCenterParams.x * projectionGeometry->horizontalPitch + blobCenterParams.y * projectionGeometry->verticalPitch - projectionGeometry->source).xyz, 0.0f);
        float4 q = (float4)((blobCenter - projectionGeometry->source).xyz, 0.0f);
        float lp = length(p.xyz);
        float lq = length(q.xyz);
        float ldx = length(projectionGeometry->horizontalPitch.xyz);
        float ldy = length(projectionGeometry->verticalPitch.xyz);

        float r = blobRadius * lp / lq;
        float alpha = asin(blobRadius / lq);
        float betaX = acos(dot(projectionGeometry->horizontalPitch.xyz, p.xyz) / (ldx * lp));
        float betaY = acos(dot(projectionGeometry->verticalPitch.xyz, p.xyz) / (ldy * lp));

        int sizeX = convert_int_rtp(max(r / sin(betaX - alpha), r / sin(betaX + alpha)) / ldx);
        int sizeY = convert_int_rtp(max(r / sin(betaY - alpha), r / sin(betaY + alpha)) / ldy);
        int2 basePixel = convert_int2_rtz(blobCenterParams + 0.5f * sign(blobCenterParams));
        //sizeX = sizeY = 32;

        float accumulatedWeights = 0.0f;
        float val = 0.0f;
        for(int dy = -sizeY; dy <= sizeY; ++dy)
        {
            for(int dx = -sizeX; dx <= sizeX; ++dx)
            {
                val += GetContributionFromPixel(basePixel + (int2)(dx, dy),
                                                MEMORY2D_PARAMS(projection),
                                                MEMORY2D_PARAMS(rayLength),
                                                &privateProjection2world,
                                                projectionGeometry->source, blobCenter,
                                                blobSampler, blobSampleCount, blobRadius * blobRadius,
                                                &accumulatedWeights);
            }
        }
        if(accumulatedWeights > EPSILON)
        {
            STORE_FLOAT(vol, id, READ_FLOAT(vol, id) + lambda * val / accumulatedWeights);
        }
    }
}
