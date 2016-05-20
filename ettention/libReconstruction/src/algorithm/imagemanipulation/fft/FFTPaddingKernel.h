#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class FFTPaddingKernel : public ImageProcessingKernel
    {
    public:
        FFTPaddingKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer, bool padWithZeroes = false);
        FFTPaddingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& desiredResolution, bool padWithZeroes = false);
        ~FFTPaddingKernel();

        void setInput(GPUMapped<Image>* source);
        void setDesiredResolution(const Vec2ui& resolution);

    protected:
        void prepareKernelArguments();

    private:
        GPUMapped<Image>* source;
    };
}

