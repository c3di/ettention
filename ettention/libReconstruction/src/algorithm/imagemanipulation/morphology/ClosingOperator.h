#pragma once

namespace ettention
{
    class DilationKernel;
    class ErosionKernel;
    class Framework;
    class Image;
    template<typename _T> class GPUMapped;

    class ClosingOperator
    {
    public:
        ClosingOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement, GPUMapped<Image>* result);
        ClosingOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement);
        virtual ~ClosingOperator();

        virtual void execute();

        virtual void setInput(GPUMapped<Image>* buffer);
        virtual void setOutput(GPUMapped<Image>* buffer);

        virtual GPUMapped<Image>* getOutput() const;

    private:
        DilationKernel *dilationKernel;
        ErosionKernel *erosionKernel;
    };
}
