#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class LocalMaximumInterface : public ComputeKernel
    {
    public:
        LocalMaximumInterface(Framework* framework, GPUMapped<Image>* source, const Vec2ui& localAreaResolution, const std::string& kernelCode, const std::string& kernelName, const std::string& kernelCaption);
        virtual ~LocalMaximumInterface();

        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getSource() const;
        GPUMapped<Image>* getResult() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();

        GPUMapped<Image>* source;
        GPUMapped<Image>* result;
        Vec2ui localResolution;
    };
}

