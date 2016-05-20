#pragma once
#include "framework/math/Vec3.h"

namespace ettention
{
    class FloatVolume;

    class ConvolutionKernelGenerator
    {
        ConvolutionKernelGenerator() = delete;

    public:
        static FloatVolume *generateGaussianBlurConvolutionKernel();

    protected:
        static FloatVolume *generateConvolutionKernelByPattern(Vec3ui resolution, const float pattern[]);

    private:
        static const float GAUSSIAN_BLUR3D_RADIUS1[27];
        static const float GAUSSIAN_BLUR3D_RADIUS1_NORM[27];
    };
}