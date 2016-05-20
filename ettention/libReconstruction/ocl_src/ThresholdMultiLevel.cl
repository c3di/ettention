#include "HalfFloatSupport.hl"
#include "ImageHandler.hl"

__kernel void thresholdMultiLevel(TYPE_3D_READONLY(source), TYPE_3D_READWRITE(result), __global float *floorLevels, __global float *floorValues, int numberOfLevels)
{
    const uint3 workerId    = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));
    const uint3 size        = (uint3)(get_global_size(0), get_global_size(1), get_global_size(2));

    const int3 pos          = (int3)((int)workerId.x, (int)workerId.y, (int)workerId.z);
    const int currentIndex  = pos.x + (pos.y * size.x) + (pos.z * size.y * size.x);

    float oldValue          = READ_3D_READONLY(source, (int4)(pos, 0));
    float currentValue      = oldValue;

    if( oldValue     < floorLevels[0])
        currentValue = floorValues[0];
    else
    {
        for(int i = 1; i < numberOfLevels; ++i)
        {
            if( (floorLevels[i - 1] <= oldValue) && (oldValue < floorLevels[i]) )
            {
                currentValue = floorValues[i];
                break;
            }
        }
    }

    if( oldValue >= floorLevels[numberOfLevels - 1] )
        currentValue = THRESHOLDING_CEILVALUE;

    WRITE_3D_READWRITE(result, currentIndex, currentValue);
}