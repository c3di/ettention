#include "stdafx.h"
#include "FFTPaddingKernel.h"
#include "PaddingForFFT_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{

    FFTPaddingKernel::FFTPaddingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& desiredResolution, bool padWithZeroes)
        : ComputeKernel(framework, PaddingForFFT_kernel_SourceCode, padWithZeroes ? "computeZeroPaddedCopyForFourierSpace" : "computePaddedCopyForFourierSpace", "FFTPaddingKernel")
    {
        setInput(source);
        createOutputBuffer(desiredResolution);
    }

    FFTPaddingKernel::~FFTPaddingKernel()
    {
        deleteOutputBuffer();
    }

    void FFTPaddingKernel::createOutputBuffer(const Vec2ui& resolution)
    {
        result = new GPUMapped<Image>(abstractionLayer, resolution);
    }

    void FFTPaddingKernel::deleteOutputBuffer()
    {
        delete result;
    }

    void FFTPaddingKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", result);
        implementation->setArgument("originalSizeX", (int)source->getResolution().x);
        implementation->setArgument("originalSizeY", (int)source->getResolution().y);
        implementation->setArgument("desiredSizeX", (int)result->getResolution().x);
        implementation->setArgument("desiredSizeY", (int)result->getResolution().y);
    }

    void FFTPaddingKernel::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(result->getResolution());
        implementation->setGlobalWorkSize(dim);
    }

    void FFTPaddingKernel::postRun()
    {
        result->markAsChangedOnGPU();
    }

    void FFTPaddingKernel::setInput(GPUMapped<Image>* source)
    {
        this->source = source;
    }

    void FFTPaddingKernel::setDesiredResolution(const Vec2ui& resolution)
    {
        if(result->getResolution() != resolution)
        {
            this->deleteOutputBuffer();
            this->createOutputBuffer(resolution);
        }
    }

    GPUMapped<Image>* FFTPaddingKernel::getResult() const
    {
        return result;
    }
}