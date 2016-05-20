#ifndef HALF_FLOAT_TEST
#define HALF_FLOAT_TEST

__kernel void halfFloatTest(__global half* data)
{
    float data0 = vload_half(0, data);
    float data1 = vload_half(1, data);
    float data2 = vload_half(2, data);
    float data3 = vload_half(3, data);
    float data4 = vload_half(4, data);
    float data5 = vload_half(5, data);

    vstore_half(data0 + data1, 0, data);
    vstore_half(data1 / 2.0f,  1, data);
    vstore_half(data2 * 2.0f,  2, data);
    vstore_half(data3 / data3, 3, data);
    vstore_half(data4 * data4, 4, data);
    vstore_half(data5        , 5, data);
}

#endif
