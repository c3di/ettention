#include "stdafx.h"
#include "algorithm/imagemanipulation/fft/FFTForwardKernel.h"
#include "framework/error/FFTException.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"

namespace ettention
{

    FFTForwardKernel::FFTForwardKernel(CLAbstractionLayer* abstractionLayer, GPUMapped<Image>* input)
        : FFTKernel(abstractionLayer, input->getResolution().xy())
        , input(input)
    {
        initializeKernel();
        initializeBuffers();
    }

    FFTForwardKernel::~FFTForwardKernel()
    {
        returnBuffers();
    }

    void FFTForwardKernel::run()
    {
        input->ensureIsUpToDateOnGPU();
        inputHandle = input->getBufferOnGPU()->getCLMem();
        outputHandle[0] = output[0]->getBufferOnGPU()->getCLMem();
        outputHandle[1] = output[1]->getBufferOnGPU()->getCLMem();
        callToFftLibrary(clAmdFftEnqueueTransform(kernelPlan, CLFFT_FORWARD, 1, &queue, 0, NULL, NULL, &inputHandle, &outputHandle[0], NULL));
        clFinish(queue);

        this->PostProcessKernelRun();
    }

    void FFTForwardKernel::PostProcessKernelRun()
    {
        this->output[0]->markAsChangedOnGPU();
        this->output[1]->markAsChangedOnGPU();

        this->output[0]->ensureIsUpToDateOnCPU();
        this->output[1]->ensureIsUpToDateOnCPU();
    }

    void FFTForwardKernel::setInput(GPUMapped<Image>* val)
    {
        this->input = val;
    }

    void FFTForwardKernel::initializeKernel()
    {
        FFTKernel::initializeKernel();

        const size_t inputDistance = baseResolution.x * baseResolution.y;
        const size_t outputDistance = baseResolution.x * baseResolution.y;
        callToFftLibrary(clAmdFftSetPlanDistance(kernelPlan, inputDistance, outputDistance));

        callToFftLibrary(clAmdFftSetLayout(kernelPlan, CLFFT_REAL, CLFFT_HERMITIAN_PLANAR));

        callToFftLibrary(clAmdFftSetPlanDistance(kernelPlan, getSizeOfInputBuffer(), getSizeOfOutputBuffer()));

        size_t iStrides[2];
        iStrides[0] = 1;
        iStrides[1] = input->getResolution().x;
        callToFftLibrary(clAmdFftSetPlanInStride(kernelPlan, CLFFT_2D, iStrides));

        size_t oStrides[2];
        oStrides[0] = 1;
        oStrides[1] = 1 + input->getResolution().x / 2;
        callToFftLibrary(clAmdFftSetPlanOutStride(kernelPlan, CLFFT_2D, oStrides));

        callToFftLibrary(clAmdFftBakePlan(kernelPlan, 1, &queue, 0, 0));
    }

    void FFTForwardKernel::initializeBuffers()
    {
        Vec2ui outputResolution = Vec2ui(baseResolution.x / 2 + 1, baseResolution.y);
        output[0] = new GPUMapped<Image>(this->clAbstractionLayer, outputResolution);
        output[1] = new GPUMapped<Image>(this->clAbstractionLayer, outputResolution);
    }

    void FFTForwardKernel::returnBuffers()
    {
        delete output[0];
        delete output[1];
    }

    GPUMapped<Image>* FFTForwardKernel::getOutputRealPart() const
    {
        return output[0];
    }

    GPUMapped<Image>* FFTForwardKernel::getOutputImaginaryPart() const
    {
        return output[1];
    }

    Image* FFTForwardKernel::getRealOutputAsImage()
    {
        return this->getOutputRealPart()->getObjectOnCPU();
    }

    Image* FFTForwardKernel::getImaginaryOutputAsImage()
    {
        return this->getOutputImaginaryPart()->getObjectOnCPU();
    }

    unsigned int FFTForwardKernel::getSizeOfInputBuffer()
    {
        return input->getResolution().x * input->getResolution().x;
    }

    unsigned int FFTForwardKernel::getSizeOfOutputBuffer()
    {
        unsigned int sizeX = input->getResolution().x;
        return ((sizeX / 2) + 1) * sizeX;
    }
}