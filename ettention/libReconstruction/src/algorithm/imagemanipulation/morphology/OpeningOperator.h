#pragma once

namespace ettention
{
    class ErosionKernel;
    class DilationKernel;
    class Framework;
    class Image;
    template<typename _T> class GPUMapped;

    class OpeningOperator
    {
    public:
        OpeningOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement, GPUMapped<Image>* result);
        OpeningOperator(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* structureElement);
        virtual ~OpeningOperator();

        virtual void execute();

        virtual void setInput(GPUMapped<Image>* buffer);
        virtual void setOutput(GPUMapped<Image>* buffer);

        virtual GPUMapped<Image>* getOutput() const;

    private:
        ErosionKernel *erosionKernel;
        DilationKernel *dilationKernel;
    };
}

