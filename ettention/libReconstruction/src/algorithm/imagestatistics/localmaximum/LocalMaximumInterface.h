#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class LocalMaximumInterface : public ImageProcessingKernel
    {
    public:
        LocalMaximumInterface(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer, const Vec2ui& localAreaResolution, const std::string& kernelCode, const std::string& kernelName, const std::string& kernelCaption);
        LocalMaximumInterface(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaSize, const std::string& kernelCode, const std::string& kernelName, const std::string& kernelCaption);
        virtual ~LocalMaximumInterface();

        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getSource() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* source;
        Vec2ui localAreaSize;
    };
}

