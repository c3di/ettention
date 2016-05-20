#include "stdafx.h"
#include "ConvolutionKernelGenerator.h"

#include "model/volume/FloatVolume.h"
#include "framework/error/Exception.h"

namespace ettention
{

    FloatVolume *ConvolutionKernelGenerator::generateGaussianBlurConvolutionKernel()
    {
        return generateConvolutionKernelByPattern(Vec3ui(3, 3, 3), GAUSSIAN_BLUR3D_RADIUS1_NORM);
    }

    FloatVolume *ConvolutionKernelGenerator::generateConvolutionKernelByPattern(Vec3ui resolution, const float pattern[])
    {
        auto *result = new FloatVolume(resolution, pattern);
        return result;
    }

    const float ConvolutionKernelGenerator::GAUSSIAN_BLUR3D_RADIUS1[] = {
        1,    8,  1,
        8,   64,  8,
        1,    8,  1,

        8,   64,  8,
        64, 512, 64,
        8,   64,  8,
        
        1,    8,  1,
        8,   64,  8,
        1,    8,  1
    };

    const float ConvolutionKernelGenerator::GAUSSIAN_BLUR3D_RADIUS1_NORM[] = {
        0.001f, 0.008f, 0.001f,
        0.008f, 0.064f, 0.008f,
        0.001f, 0.008f, 0.001f,

        0.008f, 0.064f, 0.008f,
        0.064f, 0.512f, 0.064f,
        0.008f, 0.064f, 0.008f,

        0.001f, 0.008f, 0.001f,
        0.008f, 0.064f, 0.008f,
        0.001f, 0.008f, 0.001f
    };

}