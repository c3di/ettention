#include "stdafx.h"
#include "ConvolutionRealSpaceKernel.h"
#include "ImageConvolution_bin2c.h"

#include "framework/error/ImageResolutionDiffersException.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    ConvolutionRealSpaceKernel::ConvolutionRealSpaceKernel(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source, GPUMapped<Image>* outputContainer)
        : ImageProcessingKernel(framework, outputContainer, ImageConvolution_kernel_SourceCode, "computeConvolution", "ConvolutionRealSpaceKernel")
        , projectionResolution(source->getResolution().xy())
    {
        setInputs(convolutionKernel, source);
    }

    ConvolutionRealSpaceKernel::ConvolutionRealSpaceKernel(Framework* framework, GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source)
        : ImageProcessingKernel(framework, source->getResolution(), ImageConvolution_kernel_SourceCode, "computeConvolution", "ConvolutionRealSpaceKernel")
        , projectionResolution(source->getResolution().xy())
    {
        setInputs(convolutionKernel, source);
    }

    ConvolutionRealSpaceKernel::~ConvolutionRealSpaceKernel()
    {
    }

    void ConvolutionRealSpaceKernel::prepareKernelArguments()
    {
        implementation->setArgument("convolutionKernel", this->convolutionKernel);
        implementation->setArgument("source", this->source);
        implementation->setArgument("result", this->getOutput());
        implementation->setArgument("kernelSizeX", (int)this->kernelResolution.x);
        implementation->setArgument("kernelSizeY", (int)this->kernelResolution.y);
        implementation->setArgument("sourceSizeX", (int)this->projectionResolution.x);
        implementation->setArgument("sourceSizeY", (int)this->projectionResolution.y);
        implementation->setArgument("weightSum", this->weightsSum);
    }

    void ConvolutionRealSpaceKernel::setInputs(GPUMapped<Image>* convolutionKernel, GPUMapped<Image>* source)
    {
        setKernel(convolutionKernel);
        setInput(source);
    }

    void ConvolutionRealSpaceKernel::setKernel(GPUMapped<Image>* convolutionKernel)
    {
        this->convolutionKernel = convolutionKernel;
        kernelResolution = convolutionKernel->getResolution().xy();

        float sum = 0.0f;
        for(unsigned int j = 0; j < kernelResolution.y; ++j)
        {
            for(unsigned int i = 0; i < kernelResolution.x; ++i)
            {
                sum += convolutionKernel->getObjectOnCPU()->getPixel(i, j);
            }
        }

        weightsSum = sum;
    }

    void ConvolutionRealSpaceKernel::setInput(GPUMapped<Image>* source)
    {
        if( source->getResolution().xy() != projectionResolution )
            throw ImageResolutionDiffersException(projectionResolution, source->getResolution(), "ConvolutionRealSpaceKernel::setInput");

        this->source = source;
    }

    float ConvolutionRealSpaceKernel::getWeightsSum() const
    {
        return weightsSum;
    }
}