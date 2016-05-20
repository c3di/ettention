#include "stdafx.h"
#include "OpeningOperator.h"
#include "algorithm/imagemanipulation/morphology/ErosionKernel.h"
#include "algorithm/imagemanipulation/morphology/DilationKernel.h"
#include "framework/Framework.h"

namespace ettention
{
    OpeningOperator::OpeningOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement, GPUMapped<Image>* result)
    {
        this->erosionKernel = new ErosionKernel(framework, source, structureElement, result);
        this->dilationKernel = new DilationKernel(framework, erosionKernel->getOutput(), structureElement, result);
    }

    OpeningOperator::OpeningOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement)
    {
        this->erosionKernel = new ErosionKernel(framework, source, structureElement);
        this->dilationKernel = new DilationKernel(framework, erosionKernel->getOutput(), structureElement);
    }

    OpeningOperator::~OpeningOperator()
    {
        delete this->dilationKernel;
        delete this->erosionKernel;
    }

    void OpeningOperator::execute()
    {
        this->erosionKernel->run();
        this->dilationKernel->run();
    }

    GPUMapped<Image>* OpeningOperator::getOutput() const
    {
        return this->dilationKernel->getOutput();
    }

    void OpeningOperator::setInput(GPUMapped<Image>* source)
    {
        erosionKernel->setInput(source);
    }

    void OpeningOperator::setOutput(GPUMapped<Image>* buffer)
    {
        dilationKernel->setResultContainer(buffer);
    }
}