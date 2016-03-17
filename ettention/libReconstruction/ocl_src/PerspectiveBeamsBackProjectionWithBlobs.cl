#include "HalfFloatSupport.hl"
#include "ImageHandler.hl"
#include "Math.hl"

#include "VolumeGeometry.hl"
#include "ProjectionGeometry.hl"
#include "Matrix4x4.hl"
#include "BlobRaytracer.hl"

float getContributionFromPixel(int2 pixel, 
                               TYPE_2D_READONLY(projection),
                               TYPE_2D_READONLY(rayLength),
                               Matrix4x4* projection2world,
                               float4 emitterSource,
                               float4 blobCenter,
                               __global float* blobSampler,
                               uint blobSampleCount,
                               float blobRadiusSquared,
                               float* accumulatedWeights)
{
    if(all(0 <= pixel) && all(pixel < MEMORY_SIZE(projection)))
    {
        const float len = READ_2D_READONLY(rayLength, pixel);
        const float residual = READ_2D_READONLY(projection, pixel);
        if(residual != 0.0f && len > 1.0f) // magic epsilon
        {
            float4 pixelCenter = multiply(projection2world, (float4)(convert_float2(pixel), 1.0f, 1.0f));
            float4 pixelRayDir = (float4)(normalize(pixelCenter.xyz / pixelCenter.w - emitterSource.xyz), 0.0f);
            float4 sourceToBlobCenter = (float4)(blobCenter.xyz - emitterSource.xyz, 0.0f);
            float t0 = dot(pixelRayDir, sourceToBlobCenter);
            float pixelRayToBlobCenterDistSquared = dot(sourceToBlobCenter, sourceToBlobCenter) - t0 * t0;
            if(blobRadiusSquared > pixelRayToBlobCenterDistSquared)
            {
                float blobTraversalValue = SampleBlobFromGlobal(blobSampler, blobSampleCount, sqrt(pixelRayToBlobCenterDistSquared / blobRadiusSquared));
                *accumulatedWeights += blobTraversalValue;
                return residual * blobTraversalValue / len;
            }
        }
    }
    return 0.0f;
}

__kernel void correct(TYPE_2D_READONLY(projection),
                      TYPE_3D_READWRITE(vol),
                      TYPE_3D_MASK(mask),
                      TYPE_2D_READONLY(rayLength),
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
                val += getContributionFromPixel(basePixel + (int2)(dx, dy),
                                                MEMORY_PARAMS(projection),
                                                MEMORY_PARAMS(rayLength),
                                                &privateProjection2world,
                                                projectionGeometry->source, blobCenter,
                                                blobSampler, blobSampleCount, blobRadius * blobRadius,
                                                &accumulatedWeights);
            }
        }

        if(accumulatedWeights > WEAK_EPSILON)
        {        
            float maskValue = READ_3D_MASK_IF_POSSIBLE(mask, convert_float4((uint4)(gid, 0)));
            WRITE_3D_READWRITE(vol, id, READ_3D_READWRITE(vol, id) + lambda * val * maskValue / accumulatedWeights);
        }
    }
}
