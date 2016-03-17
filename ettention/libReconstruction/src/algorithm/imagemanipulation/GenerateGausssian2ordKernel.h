#pragma once
#include "framework/math/Vec2.h"
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class GenerateGausssian2ordKernel : public ComputeKernel
    {
    public:
        GenerateGausssian2ordKernel(Framework* framework, unsigned int particleDiameter);
        GenerateGausssian2ordKernel(Framework* framework, unsigned int particleDiameter, unsigned int resultDiameter);
        ~GenerateGausssian2ordKernel();

        float getSigma() const;
        GPUMapped<Image>* getResultXX() const;
        GPUMapped<Image>* getResultXY() const;
        GPUMapped<Image>* getResultYY() const;

        Image* getResultXXAsImage() const;
        Image* getResultXYAsImage() const;
        Image* getResultYYAsImage() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void createOutputBuffer();
        void deleteOutputBuffer();
        void setSigma(unsigned int diameter);

        GPUMapped<Image>* resultXX;
        GPUMapped<Image>* resultXY;
        GPUMapped<Image>* resultYY;
        Vec2ui projectionResolution;
        unsigned int particleDiameter;
        float sigma;
    };
}

