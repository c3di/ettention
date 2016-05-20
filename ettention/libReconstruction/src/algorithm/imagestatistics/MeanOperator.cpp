#include "stdafx.h"
#include "MeanOperator.h"
#include "algorithm/imagestatistics/ColumnAverageKernel.h"
#include "algorithm/imagestatistics/MeanKernel.h"

namespace ettention
{
    MeanOperator::MeanOperator(Framework* framework, GPUMapped<Image>* source)
    {
        this->columnMeanKernel = new ColumnAverageKernel(framework, source);
        this->bufferMeanKernel = new MeanKernel(framework, columnMeanKernel->getColumnAverage());
    }

    MeanOperator::~MeanOperator()
    {
        delete this->columnMeanKernel;
        delete this->bufferMeanKernel;
    }

    void MeanOperator::setInput(GPUMapped<Image> *buffer)
    {
        this->columnMeanKernel->setInput(buffer);
    }

    void MeanOperator::computeMean()
    {
        this->columnMeanKernel->run();
        this->bufferMeanKernel->run();
        this->mean = this->bufferMeanKernel->getMean();
    }

    float MeanOperator::getMean()
    {
        return this->mean;
    }
}