#pragma once

namespace ettention
{
    class CLAbstractionLayer;
    class Framework;
    class Image;
    class PhaseRandomizationOperator;
    template<typename _T> class GPUMapped;

    class MissingWedgeImageGenerator
    {
    public:
        MissingWedgeImageGenerator(Framework* framework, GPUMapped<Image>* inputImage);
        ~MissingWedgeImageGenerator();

        Image* getOutput();
        void run();

    private:
        CLAbstractionLayer* abstractionLayer;
        PhaseRandomizationOperator* phaseRandomizationOperator;

        GPUMapped<Image>* inputImage;
        Image* randomImage;
    };
}

