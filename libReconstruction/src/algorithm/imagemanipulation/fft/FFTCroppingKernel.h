#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class FFTCroppingKernel : public ComputeKernel
    {
    public:
        FFTCroppingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& outputResolution);
        ~FFTCroppingKernel();

        void setInput(GPUMapped<Image>* input);
        void setOutput(GPUMapped<Image>* output);
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
        bool ownsOutput;
        Vec2ui outputResolution;
    };
}

