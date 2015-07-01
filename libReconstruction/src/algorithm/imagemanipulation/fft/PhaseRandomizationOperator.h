#pragma once

namespace ettention
{
    class CLAbstractionLayer;
    class FFTBackKernel;
    class FFTForwardKernel;
    class FFTMagnitudeKernel;
    class FFTPhaseKernel;
    class Framework;
    class Image;
    class PhaseRandomizationKernel;
    template<typename _T> class GPUMapped;

    class PhaseRandomizationOperator
    {
    public:
        PhaseRandomizationOperator(Framework* framework, GPUMapped<Image>* imageInput);
        virtual ~PhaseRandomizationOperator();

        void run();
        void setRandomPhase(unsigned int seed = static_cast<unsigned int>(std::time(0)));

        Image* getOutput();

    protected:
        GPUMapped<Image>* imageWithRandomizedPhase;

    private:
        CLAbstractionLayer* abstractionLayer;

        unsigned int seedTime;
        GPUMapped<Image>* imageInput;
        GPUMapped<Image>* randomImage;

        FFTForwardKernel* fftForwardKernel;
        FFTMagnitudeKernel* fftMagnitudeKernel;
        PhaseRandomizationKernel* phaseRandomizationKernel;
        FFTBackKernel* fftBackKernel;
    };
}
