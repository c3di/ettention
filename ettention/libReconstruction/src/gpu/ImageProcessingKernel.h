#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Framework;

    template<typename _T> class Vec2;
    class Image;
    template<typename _T> class GPUMapped;

    class ImageProcessingKernel : public ComputeKernel
    {
    public:
        ImageProcessingKernel(Framework* framework, GPUMapped<Image> *outputContainer, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, const std::string& additionalKernelDefines = "");
        ImageProcessingKernel(Framework* framework, const Vec2ui& outputSize, const std::string& sourceCode, const std::string& kernelName, const std::string& kernelCaption, const std::string& additionalKernelDefines = "");
        virtual ~ImageProcessingKernel();

        virtual void setResultContainer(GPUMapped<Image>* resultContainer);
        virtual void allocateResultContainer(Vec2ui size);
        virtual void releaseResultContainer();

        void preRun() override;
        void postRun() override;

        virtual GPUMapped<Image>* getOutput() const;

    protected:
        bool ownResultContainer = false;

    private:
        GPUMapped<Image> *resultContainer = nullptr;

    };
}
