#pragma once
#include "algorithm/imagestatistics/globalmaximum/GlobalMaximumPositionInterface.h"
#include "gpu/ComputeKernel.h"
#include "framework/math/Vec2.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class GlobalMaximumPositionImageKernel : public GlobalMaximumPositionInterface
    {
    public:
        GlobalMaximumPositionImageKernel(Framework* framework, GPUMapped<Image>* sourceImage);
        ~GlobalMaximumPositionImageKernel();

        void setInputImage(GPUMapped<Image>* sourceImage);
        Vec2ui getMaximumXY() const;

    private:
        GPUMapped<Image>* sourceImage;
        Vec2ui resolution;
    };
}