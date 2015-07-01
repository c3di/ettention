#pragma once

#include "algorithm/imagemanipulation/fft/FFTKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    template<typename _T> class GPUMapped;

    class FFTBackKernel : public FFTKernel
    {
    public:
        FFTBackKernel(CLAbstractionLayer* abstractionLayer, GPUMapped<Image>* inputRealPart, GPUMapped<Image>* inputImaginaryPart);
        virtual ~FFTBackKernel();

        virtual void run();
        virtual void PostProcessKernelRun();

        void SetInputRealPart(GPUMapped<Image>* val);
        void SetInputImaginaryPart(GPUMapped<Image>* val);
        void SetInputResolution(Vec2ui inputResolution);

        GPUMapped<Image>* getOutput() const;

    protected:
        void initializeKernel();
        void initializeBuffers();
        void returnBuffers();

        unsigned int getSizeOfInput();
        unsigned int getSizeOfOutput();
        Vec2ui computeOutputResolution(Vec2ui inputResolution);

    protected:
        size_t tmpBufferSize;
        cl_mem tmpBuffer;

        GPUMapped<Image>* input[2];
        cl_mem inputHandle[2];

        GPUMapped<Image>* output;
        cl_mem outputHandle;
    };
}