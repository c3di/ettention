#pragma once

namespace ettention
{
    class CLAbstractionLayer;
    class ColumnAverageKernel;
    class Framework;
    class Image;
    class MeanKernel;
    template<typename _T> class GPUMapped;

    class MeanOperator
    {
    public:
        MeanOperator(Framework* framework, GPUMapped<Image>* source);
        virtual ~MeanOperator();
        
        void setInput(GPUMapped<Image>* source);
        void computeMean();
        float getMean();

    private:
        CLAbstractionLayer* abstractionLayer;
        ColumnAverageKernel* columnMeanKernel;
        MeanKernel *bufferMeanKernel;
        float mean;
    };
}
