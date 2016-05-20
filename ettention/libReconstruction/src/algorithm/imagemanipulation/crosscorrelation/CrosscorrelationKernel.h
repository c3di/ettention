#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class CrosscorrelationKernel : public ImageProcessingKernel
    {
    public:
        CrosscorrelationKernel(Framework* framework,
                               const std::string& sourceCode,
                               const std::string& kernelName,
                               const std::string& kernelCaption,
                               GPUMapped<Image> *reference,
                               GPUMapped<Image> *candidate,
                               float candidateMean);
        CrosscorrelationKernel(Framework* framework,
                               const std::string& sourceCode,
                               const std::string& kernelName,
                               const std::string& kernelCaption,
                               GPUMapped<Image> *reference,
                               GPUMapped<Image> *candidate,
                               GPUMapped<Image> *outputContainer,
                               float candidateMean);
        virtual ~CrosscorrelationKernel();

        void setInputs(GPUMapped<Image>* reference, GPUMapped<Image>* candidate);
        void setInputReference(GPUMapped<Image>* buffer);
        void setInputCandidate(GPUMapped<Image>* buffer);
        void setCandidateMean(float candidateMean);

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* reference;
        GPUMapped<Image>* candidate;
        Vec2ui projectionResolution;
        Vec2ui patchResolution;
        float candidateMean;

    };
}