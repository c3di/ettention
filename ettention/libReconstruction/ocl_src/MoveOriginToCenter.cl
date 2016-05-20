#include "Math.hl"

__kernel void computeShiftedToCenterOrigin(__global float *src, __global float *result, int sizeX,  int sizeY)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
    const unsigned int i = y * sizeX + x;

    int cx = (x + (sizeX / 2)) % sizeX;
    int cy = (y + (sizeY / 2)) % sizeY;
    int position =  cy * sizeX + cx;

    result[i] = src[position];
}