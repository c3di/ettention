#ifndef EIGEN_ABBC_CL
#define EIGEN_ABBC_CL

#include "Math.hl"

// Compute Eigenvalues of 2x2 Matrix of Images with equal elements on antidiagonal [A B B C] and return separate Image for Min Eigenvalue and Max Eigenvalue
__kernel void compute2x2MatrixEigenvalues(__global float *A, __global float *B, __global float *C, __global float *minLambda, __global float *maxLambda, int sizeX, int sizeY)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);

    if (x >= sizeX || y >= sizeY)
        return;

    const unsigned int i = y * sizeX + x;
    minLambda[i] = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE;
    maxLambda[i] = THE_ANSWER_TO_THE_ULTIMATE_QUESTION_OF_LIFE;

    float a = A[i];
    float b = B[i];
    float c = C[i];

    float summ = a + c;
    float diff = a - c;
    float db = 2 * b;
    float discriminant = (diff - db)*(diff + db);
    if(discriminant < 0.0f) // WTF?!
    {
        discriminant = 0.0f;
    }

    float delta = sqrt(discriminant);
    maxLambda[i] = (summ + delta) / 2;
    minLambda[i] = (summ - delta) / 2;
}

#endif
