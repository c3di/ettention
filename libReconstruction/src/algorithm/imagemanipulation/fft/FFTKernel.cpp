#include "stdafx.h"
#include "algorithm/imagemanipulation/fft/FFTKernel.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "clAmdFft.h"
#include "framework/error/FFTException.h"

namespace ettention
{
    unsigned int FFTKernel::kernelCount = 0;
    clAmdFftSetupData* FFTKernel::setupData = NULL;

    FFTKernel::FFTKernel(CLAbstractionLayer* abstractionLayer, Vec2ui inputResolution)
        : clAbstractionLayer(abstractionLayer)
        , baseResolution(inputResolution)
    {
        initializeLibraryIfRequired();

        queue = clAbstractionLayer->getCommandQueue();
    }

    FFTKernel::~FFTKernel()
    {
        callToFftLibrary(clAmdFftDestroyPlan(&kernelPlan));
        closeLibraryIfRequired();
    }

    void FFTKernel::initializeKernel()
    {
        const size_t clLengths[2] = { baseResolution.x, baseResolution.y };

        callToFftLibrary(clAmdFftCreateDefaultPlan(&kernelPlan, clAbstractionLayer->getContext(), CLFFT_2D, clLengths));

        callToFftLibrary(clAmdFftSetResultLocation(kernelPlan, CLFFT_OUTOFPLACE));
    }

    void FFTKernel::callToFftLibrary(clAmdFftStatus status)
    {
        if(status != clAmdFftStatus::CLFFT_SUCCESS)
            throw FFTException(status);
    }

    void FFTKernel::initializeLibraryIfRequired()
    {
        if(kernelCount == 0)
        {
            setupData = new clAmdFftSetupData();
            clAmdFftStatus status = clAmdFftInitSetupData(setupData);
            if(status != CLFFT_SUCCESS)
                throw FFTException(status);
            status = clAmdFftSetup(setupData);
            if(status != CLFFT_SUCCESS)
                throw FFTException(status);
        }
        kernelCount++;
    }

    void FFTKernel::closeLibraryIfRequired()
    {
        kernelCount--;
        if(kernelCount == 0)
        {
            auto status = clAmdFftTeardown();
            if(status != CLFFT_SUCCESS)
            {
                throw FFTException(status);
            }
            delete setupData;
        }
    }
}