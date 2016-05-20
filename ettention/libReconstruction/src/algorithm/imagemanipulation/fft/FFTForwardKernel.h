#pragma once

#include <memory>

#include "algorithm/imagemanipulation/fft/FFTKernel.h"
#include "model/image/Image.h"

#include "clAmdFft.h"
#include "gpu/GPUMapped.h"

namespace ettention
{
    class FFTForwardKernel : public FFTKernel
    {
    public:
        FFTForwardKernel(CLAbstractionLayer* abstractionLayer, GPUMapped<Image>* input);
        virtual ~FFTForwardKernel();

        virtual void run();
        virtual void PostProcessKernelRun();

        void setInput(GPUMapped<Image>* val);
        void SetInputResolution(Vec2ui inputResolution);
        GPUMapped<Image>* getOutputRealPart() const;
        GPUMapped<Image>* getOutputImaginaryPart() const;

        Image* getRealOutputAsImage();
        Image* getImaginaryOutputAsImage();

    protected:
        void initializeKernel();
        void initializeBuffers();
        void returnBuffers();

        unsigned int getSizeOfInputBuffer();
        unsigned int getSizeOfOutputBuffer();

    protected:
        GPUMapped<Image>* input;
        GPUMapped<Image>* output[2];
        cl_mem inputHandle;
        cl_mem outputHandle[2];
    };
}