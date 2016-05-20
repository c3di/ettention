#include "stdafx.h"
#include "FFTPaddingKernel.h"
#include "PaddingForFFT_bin2c.h"

#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{

    FFTPaddingKernel::FFTPaddingKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer, bool padWithZeroes)
        : ImageProcessingKernel(framework, outputContainer, PaddingForFFT_kernel_SourceCode, padWithZeroes ? "computeZeroPaddedCopyForFourierSpace" : "computePaddedCopyForFourierSpace", "FFTPaddingKernel")
    {
        setInput(source);
    }

    FFTPaddingKernel::FFTPaddingKernel(Framework* framework, GPUMapped<Image>* source, const Vec2ui& desiredResolution, bool padWithZeroes)
        : ImageProcessingKernel(framework, desiredResolution, PaddingForFFT_kernel_SourceCode, padWithZeroes ? "computeZeroPaddedCopyForFourierSpace" : "computePaddedCopyForFourierSpace", "FFTPaddingKernel")
    {
        setInput(source);
    }

    FFTPaddingKernel::~FFTPaddingKernel()
    {
    }

    void FFTPaddingKernel::prepareKernelArguments()
    {
        implementation->setArgument("src", source);
        implementation->setArgument("result", getOutput());
        implementation->setArgument("originalSizeX", (int)source->getResolution().x);
        implementation->setArgument("originalSizeY", (int)source->getResolution().y);
        implementation->setArgument("desiredSizeX", (int)getOutput()->getResolution().x);
        implementation->setArgument("desiredSizeY", (int)getOutput()->getResolution().y);
    }

    void FFTPaddingKernel::setInput(GPUMapped<Image>* source)
    {
        this->source = source;
    }

    void FFTPaddingKernel::setDesiredResolution(const Vec2ui& resolution)
    {
        if( getOutput()->getResolution() != resolution )
        {
            if(this->ownResultContainer)
            {
                this->releaseResultContainer();
                this->allocateResultContainer(resolution);
            } else {
                throw ImageResolutionDiffersException(getOutput()->getResolution(), resolution, "FFTPaddingKernel::setDesiredResolution [not owning result container]");
            }
        }
    }
}