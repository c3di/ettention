#pragma once

namespace ettention
{
    class CLAbstractionLayer;
    class Framework;
    class Image;
    class MeanOperator;
    class SquareKernel;
    template<typename _T> class GPUMapped;

    class DeviationOperator
    {
    public:
        DeviationOperator(Framework* framework, GPUMapped<Image>* source);
        virtual ~DeviationOperator();
        
        void setInput(GPUMapped<Image>* source);
        void computeDeviation();
        void computeDeviation(float knownMean);
        float getDeviation();

    private:
        CLAbstractionLayer* abstractionLayer;
        MeanOperator* meanOperator;
        SquareKernel* squareKernel;
        MeanOperator* squaredMeanOperator;
        float deviation;
    };
}
