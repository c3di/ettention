#include "Math.hl"

void __kernel execute(__global float* input,
                      __global float* output,
                      __global float* kernelValues,
                      const uint3 kernelRes)
{ 
    const uint3 outputBaseCoords = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));
    const uint3 outputRes = (uint3)(get_global_size(0), get_global_size(1), get_global_size(2));
    const uint3 inputRes = outputRes + kernelRes - 1;
    
    float val = 0.0f;
    uint3 kernelCoords;
    for(kernelCoords.z = 0; kernelCoords.z < kernelRes.z; ++kernelCoords.z)
    {
        for(kernelCoords.y = 0; kernelCoords.y < kernelRes.y; ++kernelCoords.y)
        {
            for(kernelCoords.x = 0; kernelCoords.x < kernelRes.x; ++kernelCoords.x)
            {
                const uint kernelId = Flatten3D(kernelCoords, kernelRes);
                const uint inputId = Flatten3D(outputBaseCoords + kernelCoords, inputRes);
                val += kernelValues[kernelId] * input[inputId];
            }
        }
    }
    const uint outputId = Flatten3D(outputBaseCoords, outputRes);
    output[outputId] = val;
}