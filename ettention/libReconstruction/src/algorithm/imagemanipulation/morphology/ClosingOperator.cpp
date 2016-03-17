#include "stdafx.h"
#include "ClosingOperator.h"
#include "algorithm/imagemanipulation/morphology/DilationKernel.h"
#include "algorithm/imagemanipulation/morphology/ErosionKernel.h"
#include "framework/Framework.h"

namespace ettention
{
    ClosingOperator::ClosingOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement, GPUMapped<Image>* result)
    {
        this->dilationKernel = new DilationKernel(framework, source, structureElement, result);
        this->erosionKernel = new ErosionKernel(framework, dilationKernel->getOutput(), structureElement, result);
    }

    ClosingOperator::ClosingOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement)
    {
        this->dilationKernel = new DilationKernel(framework, source, structureElement);
        this->erosionKernel = new ErosionKernel(framework, dilationKernel->getOutput(), structureElement);
    }

    ClosingOperator::~ClosingOperator()
    {
        delete this->erosionKernel;
        delete this->dilationKernel;
    }

    void ClosingOperator::execute()
    {
        this->dilationKernel->run();
        this->erosionKernel->run();
    }

    GPUMapped<Image>* ClosingOperator::getOutput() const
    {
        return this->erosionKernel->getOutput();
    }

    void ClosingOperator::setInput(GPUMapped<Image>* source)
    {
        dilationKernel->setInput(source);
    }

    void ClosingOperator::setOutput(GPUMapped<Image>* buffer)
    {
        erosionKernel->setResultContainer(buffer);
    }
}