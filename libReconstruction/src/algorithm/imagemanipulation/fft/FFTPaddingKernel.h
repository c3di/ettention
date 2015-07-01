#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class FFTPaddingKernel : public ComputeKernel
    {
    public:
        FFTPaddingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& desiredResolution, bool padWithZeroes = false);
        ~FFTPaddingKernel();

        void setInput(GPUMapped<Image>* source);
        void setDesiredResolution(const Vec2ui& resolution);
        GPUMapped<Image>* getResult() const;

    protected:
        void prepareKernelArguments();
        void preRun();
        void postRun();

    private:
        void createOutputBuffer(const Vec2ui& resolution);
        void deleteOutputBuffer();

        GPUMapped<Image>* source;
        GPUMapped<Image>* result;
    };
}

