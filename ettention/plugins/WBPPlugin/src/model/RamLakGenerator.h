#pragma once
#include <framework/math/Vec2.h>

namespace ettention
{
    class CLAbstractionLayer;
    class Image;
    template<typename _T> class GPUMapped;

    namespace wbp
    {
        class RamLakGenerator
        {
        private:
            RamLakGenerator() = delete;

        public:
            static GPUMapped<Image>* generateKernel(CLAbstractionLayer* clStack, const Vec2ui& kernelResolution);
        };
    }
}