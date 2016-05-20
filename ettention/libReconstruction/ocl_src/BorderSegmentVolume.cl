#include "HalfFloatSupport.hl"
#include "ImageHandler.hl"

#ifndef BORDERSEGMENTVOLUME_BORDER_VALUE
    #error BORDERSEGMENTVOLUME:BORDER_VALUE is not defined
#endif

__kernel void segmentBorderOnVolume(TYPE_3D_READONLY(source), TYPE_BUFFER_BYTE(result))
{
    const uint3 workerId    = (uint3)(get_global_id(0), get_global_id(1), get_global_id(2));
    const uint3 size        = (uint3)(get_global_size(0), get_global_size(1), get_global_size(2));

    const int3 pos          = (int3)((int)workerId.x, (int)workerId.y, (int)workerId.z); 
    const int currentIndex  = pos.x + (pos.y * size.x) + (pos.z * size.y * size.x);

    float currentValue      = READ_3D_READONLY(source, (int4)(pos, 0));
    bool onBorder           = false;
    uchar value             = 0;

    int3 tempPos;
    int tempIndex;
    for(int k = -1; k <= 1; ++k)
    {
        for(int j = -1; j <= 1; ++j)
        {
            for(int i = -1; i <= 1; ++i)
            {
                tempPos = pos + (int3)(i, j, k);
                if( (tempPos.x < 0) || (tempPos.x >= size.x) 
                 || (tempPos.y < 0) || (tempPos.y >= size.y) 
                 || (tempPos.z < 0) || (tempPos.z >= size.z) 
                )
                {
                    continue;
                }

                tempIndex = tempPos.x + (tempPos.y * size.x) + (tempPos.z * size.y * size.x);
                if( READ_3D_READONLY(source, (int4)(tempPos, 0)) != currentValue )
                {
                    onBorder = true;
                }
            }
        }
    }

    if( onBorder )
        value = BORDERSEGMENTVOLUME_BORDER_VALUE;

    WRITE_3D_READWRITE(result, currentIndex, value);
}