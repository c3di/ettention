#pragma once
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class CrosscorrelationKernel : public ComputeKernel
    {
    public:
        CrosscorrelationKernel(Framework* framework,
                               const std::string& sourceCode,
                               const std::string& kernelName,
                               const std::string& kernelCaption,
                               GPUMapped<Image> *reference,
                               GPUMapped<Image> *candidate,
                               float candidateMean);
        virtual ~CrosscorrelationKernel();

        void setInputs(GPUMapped<Image>* reference, GPUMapped<Image>* candidate);
        void setInputReference(GPUMapped<Image>* buffer);
        void setInputCandidate(GPUMapped<Image>* buffer);
        void setCandidateMean(float candidateMean);
        GPUMapped<Image>* getResult() const;

    protected:
        void prepareKernelArguments() override;
        void preRun() override;
        void postRun() override;

    private:
        void allocateOutput();
        void releaseOutput();

        GPUMapped<Image>* reference;
        GPUMapped<Image>* candidate;
        GPUMapped<Image>* result;
        Vec2ui projectionResolution;
        Vec2ui patchResolution;
        float candidateMean;
    };
}