#include "stdafx.h"
#include "algorithm/imagemanipulation/fft/FFTBackKernel.h"
#include "framework/error/FFTException.h"
#include "gpu/GPUMapped.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "clAmdFft.h"

namespace ettention
{
    FFTBackKernel::FFTBackKernel(CLAbstractionLayer* abstractionLayer, GPUMapped<Image>* inputRealPart, GPUMapped<Image>* inputImaginaryPart)
        : FFTKernel(abstractionLayer, computeOutputResolution(inputRealPart->getResolution().xy()))
    {
        if(inputRealPart->getResolution() != inputImaginaryPart->getResolution())
            throw Exception("Error in FFT: real and imaginary input must have same extend");

        input[0] = inputRealPart;
        input[1] = inputImaginaryPart;

        initializeKernel();
        initializeBuffers();
    }

    FFTBackKernel::~FFTBackKernel()
    {
        returnBuffers();
    }

    void FFTBackKernel::run()
    {
        input[0]->ensureIsUpToDateOnGPU();
        input[1]->ensureIsUpToDateOnGPU();

        inputHandle[0] = input[0]->getBufferOnGPU()->getCLMem();
        inputHandle[1] = input[1]->getBufferOnGPU()->getCLMem();
        outputHandle = output->getBufferOnGPU()->getCLMem();

        callToFftLibrary(clAmdFftEnqueueTransform(kernelPlan, CLFFT_BACKWARD, 1, &queue, 0, 0, NULL, inputHandle, &outputHandle, NULL));
        clFinish(queue);

        this->PostProcessKernelRun();
    }

    void FFTBackKernel::PostProcessKernelRun()
    {
        this->output->markAsChangedOnGPU();
    }

    void FFTBackKernel::SetInputRealPart(GPUMapped<Image>* val)
    {
        input[0] = val;
    }

    void FFTBackKernel::SetInputImaginaryPart(GPUMapped<Image>* val)
    {
        input[1] = val;
    }

    void FFTBackKernel::initializeKernel()
    {
        FFTKernel::initializeKernel();

        callToFftLibrary(clAmdFftSetLayout(kernelPlan, CLFFT_HERMITIAN_PLANAR, CLFFT_REAL));

        const size_t inputDistance = input[0]->getResolution().x * input[0]->getResolution().y;
        const size_t outputDistance = baseResolution.x * baseResolution.y;
        callToFftLibrary(clAmdFftSetPlanDistance(kernelPlan, inputDistance, outputDistance));

        size_t iStrides[2];
        iStrides[0] = 1;
        iStrides[1] = input[0]->getResolution().x;
        callToFftLibrary(clAmdFftSetPlanInStride(kernelPlan, CLFFT_2D, iStrides));

        size_t oStrides[2];
        oStrides[0] = 1;
        oStrides[1] = baseResolution.x;
        callToFftLibrary(clAmdFftSetPlanOutStride(kernelPlan, CLFFT_2D, oStrides));

        callToFftLibrary(clAmdFftBakePlan(kernelPlan, 1, &queue, 0, 0));
    }

    void FFTBackKernel::initializeBuffers()
    {
        output = new GPUMapped<Image>(clAbstractionLayer, baseResolution);
    }

    void FFTBackKernel::returnBuffers()
    {
        delete output;
    }

    GPUMapped<Image>* FFTBackKernel::getOutput() const
    {
        return output;
    }

    unsigned int FFTBackKernel::getSizeOfInput()
    {
        return input[0]->getResolution().x * input[0]->getResolution().y;
    }

    unsigned int FFTBackKernel::getSizeOfOutput()
    {
        Vec2ui outputSize = computeOutputResolution(input[0]->getResolution());
        return outputSize.x * outputSize.y;
    }

    Vec2ui FFTBackKernel::computeOutputResolution(Vec2ui inputResolution)
    {
        Vec2ui outputResolution;
        outputResolution.x = (inputResolution.x - 1) * 2;
        outputResolution.y = inputResolution.y;
        return outputResolution;
    }
}