#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class FFTCroppingKernel : public ImageProcessingKernel
    {
    public:
        FFTCroppingKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer);
        FFTCroppingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& outputResolution);
        ~FFTCroppingKernel();

        void setInput(GPUMapped<Image>* source);

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* source;
        Vec2ui outputResolution;
    };
}