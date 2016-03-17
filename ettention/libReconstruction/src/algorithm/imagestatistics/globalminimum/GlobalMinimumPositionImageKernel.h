#pragma once
#include "algorithm/imagestatistics/globalminimum/GlobalMinimumPositionInterface.h"
#include "gpu/ComputeKernel.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class GlobalMinimumPositionImageKernel : public GlobalMinimumPositionInterface
    {
    public:
        GlobalMinimumPositionImageKernel(Framework* framework, GPUMapped<Image>* sourceImage);
        ~GlobalMinimumPositionImageKernel();

        void setInputImage(GPUMapped<Image>* sourceImage);
        Vec2ui getMinimumXY() const;

    private:
        GPUMapped<Image>* sourceImage;
        Vec2ui resolution;
    };
}