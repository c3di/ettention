#include "HalfFloatSupport.hl"
#include "ImageHandler.hl"
#include "Math.hl"

void __kernel execute(TYPE_3D_READONLY(input),
                      TYPE_3D_READWRITE(output),
                      TYPE_3D_READONLY(kernelValues),
                      TYPE_3D_MASK(mask),
                      const uint3 kernelRes)
{ 
    const uint4 outputBaseCoords = (uint4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);
    const int4 outputBaseCoordsInt = convert_int4(outputBaseCoords);
    
    const uint3 outputRes = (uint3)(get_global_size(0), get_global_size(1), get_global_size(2));
   
    float value = 0.0f;
    int4 kernelCoords;
    kernelCoords.w = 0;
    for(kernelCoords.z = 0; kernelCoords.z < kernelRes.z; ++kernelCoords.z)
    {
        for(kernelCoords.y = 0; kernelCoords.y < kernelRes.y; ++kernelCoords.y)
        {
            for(kernelCoords.x = 0; kernelCoords.x < kernelRes.x; ++kernelCoords.x)
            {
                value += READ_3D_READONLY(kernelValues, kernelCoords) * READ_3D_READONLY(input, outputBaseCoordsInt + kernelCoords);
            }
        }
    }
    float currentValue = READ_3D_READONLY(input, outputBaseCoordsInt);
    const uint outputId = Flatten3D(outputBaseCoords.xyz, outputRes);
    WRITE_3D_IF_MASK_ALLOWS(output, outputId, value, READ_3D_MASK_IF_POSSIBLE(mask, outputBaseCoordsInt), currentValue);
}