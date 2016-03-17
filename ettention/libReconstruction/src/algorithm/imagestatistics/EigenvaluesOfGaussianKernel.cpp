#include "stdafx.h"
#include "EigenvaluesOfGaussianKernel.h"
#include "EigenValues2x2_bin2c.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    EigenvaluesOfGaussianKernel::EigenvaluesOfGaussianKernel(Framework* framework, GPUMapped<Image>* sourceA, GPUMapped<Image>* sourceB, GPUMapped<Image>* sourceC)
        : ComputeKernel(framework, EigenValues2x2_kernel_SourceCode, "compute2x2MatrixEigenvalues", "EigenvaluesOfGaussianKernel")
        , sourceA(0)
        , sourceB(0)
        , sourceC(0)
        , resultMins(0)
        , resultMaxs(0)
    {
        setInput(sourceA, sourceB, sourceC);
    }

    EigenvaluesOfGaussianKernel::~EigenvaluesOfGaussianKernel()
    {
        deleteOutputBuffer();
    }

    void EigenvaluesOfGaussianKernel::setInput(GPUMapped<Image>* sourceA, GPUMapped<Image>* sourceB, GPUMapped<Image>* sourceC)
    {
        if((sourceA->getResolution() != sourceB->getResolution()) || (sourceB->getResolution() != sourceC->getResolution()))
            throw Exception("EigenvaluesOfGaussianKernel: Source images are differs in size!");
        
        if(!this->sourceA || this->sourceA->getResolution() != sourceA->getResolution())
        {
            this->sourceA = sourceA;
            this->sourceB = sourceB;
            this->sourceC = sourceC;
            deleteOutputBuffer();
            createOutputBuffer();
        }
    }

    void EigenvaluesOfGaussianKernel::prepareKernelArguments()
    {
        implementation->setArgument("A", sourceA);
        implementation->setArgument("B", sourceB);
        implementation->setArgument("C", sourceC);
        implementation->setArgument("minLambda", resultMins);
        implementation->setArgument("maxLambda", resultMaxs);
        implementation->setArgument("sizeX", (int)sourceA->getResolution().x);
        implementation->setArgument("sizeY", (int)sourceA->getResolution().y);
    }

    void EigenvaluesOfGaussianKernel::preRun()
    {
        auto dim = NumericalAlgorithms::nearestPowerOfTwo(sourceA->getResolution());
        implementation->setGlobalWorkSize(dim);
    }

    void EigenvaluesOfGaussianKernel::postRun()
    {
        resultMins->markAsChangedOnGPU();
        resultMaxs->markAsChangedOnGPU();
    }

    void EigenvaluesOfGaussianKernel::createOutputBuffer()
    {
        resultMins = new GPUMapped<Image>(abstractionLayer, sourceA->getResolution());
        resultMaxs = new GPUMapped<Image>(abstractionLayer, sourceA->getResolution());
    }

    void EigenvaluesOfGaussianKernel::deleteOutputBuffer()
    {
        delete resultMins;
        delete resultMaxs;
    }

    GPUMapped<Image>* EigenvaluesOfGaussianKernel::getResultMins() const
    {
        return resultMins;
    }
    GPUMapped<Image>* EigenvaluesOfGaussianKernel::getResultMaxs() const
    {
        return resultMaxs;
    }
}