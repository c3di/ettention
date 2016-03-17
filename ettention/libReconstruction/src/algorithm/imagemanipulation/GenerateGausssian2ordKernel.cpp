#include "stdafx.h"
#include "GenerateGausssian2ordKernel.h"
#include "Gaussian2ord_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    GenerateGausssian2ordKernel::GenerateGausssian2ordKernel(Framework* framework, unsigned int particleDiameter)
        : ComputeKernel(framework, Gaussian2ord_kernel_SourceCode, "computeSecondOrderDirectionalGaussian", "GenerateGausssian2ordKernel")
        , projectionResolution(Vec2ui(2 * particleDiameter + 1, 2 * particleDiameter + 1))
        , particleDiameter(particleDiameter)
    {
        setSigma(particleDiameter);
        createOutputBuffer();
    }

    GenerateGausssian2ordKernel::GenerateGausssian2ordKernel(Framework* framework, unsigned int particleDiameter, unsigned int resultDiameter)
        : ComputeKernel(framework, Gaussian2ord_kernel_SourceCode, "computeSecondOrderDirectionalGaussian", "GenerateGausssian2ordKernel")
        , projectionResolution(Vec2ui(resultDiameter, resultDiameter))
        , particleDiameter(particleDiameter)
    {
        setSigma(particleDiameter);
        createOutputBuffer();
    }

    GenerateGausssian2ordKernel::~GenerateGausssian2ordKernel()
    {
        deleteOutputBuffer();
    }

    void GenerateGausssian2ordKernel::setSigma(unsigned int particleDiameter)
    {
        sigma = ((float)particleDiameter) / (2.0f * sqrt(2.0f)); // sigma = radius / sqrt(2);
    }

    float GenerateGausssian2ordKernel::getSigma() const
    {
        return sigma;
    }

    void GenerateGausssian2ordKernel::prepareKernelArguments()
    {
        implementation->setArgument("GsXX", resultXX);
        implementation->setArgument("GsXY", resultXY);
        implementation->setArgument("GsYY", resultYY);
        implementation->setArgument("sizeX", (int)projectionResolution.x);
        implementation->setArgument("sizeY", (int)projectionResolution.y);
        implementation->setArgument("sigma", sigma);
        implementation->setArgument("particleDiameter", (int)particleDiameter);
    }

    void GenerateGausssian2ordKernel::preRun()
    {
        Vec2ui dim = NumericalAlgorithms::nearestPowerOfTwo(projectionResolution);
        implementation->setGlobalWorkSize(dim);
    }

    void GenerateGausssian2ordKernel::postRun()
    {
        resultXX->markAsChangedOnGPU();
        resultXY->markAsChangedOnGPU();
        resultYY->markAsChangedOnGPU();
    }

    void GenerateGausssian2ordKernel::createOutputBuffer()
    {
        resultXX = new GPUMapped<Image>(abstractionLayer, projectionResolution);
        resultXY = new GPUMapped<Image>(abstractionLayer, projectionResolution);
        resultYY = new GPUMapped<Image>(abstractionLayer, projectionResolution);
    }

    void GenerateGausssian2ordKernel::deleteOutputBuffer()
    {
        delete resultXX;
        delete resultXY;
        delete resultYY;
    }

    GPUMapped<Image> *GenerateGausssian2ordKernel::getResultXX() const
    {
        return resultXX;
    }

    GPUMapped<Image> *GenerateGausssian2ordKernel::getResultXY() const
    {
        return resultXY;
    }

    GPUMapped<Image> *GenerateGausssian2ordKernel::getResultYY() const
    {
        return resultYY;
    }

    Image *GenerateGausssian2ordKernel::getResultXXAsImage() const
    {
        resultXX->ensureIsUpToDateOnCPU();
        return resultXX->getObjectOnCPU();
    }
    Image *GenerateGausssian2ordKernel::getResultXYAsImage() const
    {
        resultXY->ensureIsUpToDateOnCPU();
        return resultXY->getObjectOnCPU();
    }
    Image *GenerateGausssian2ordKernel::getResultYYAsImage() const
    {
        resultYY->ensureIsUpToDateOnCPU();
        return resultYY->getObjectOnCPU();
    }

}