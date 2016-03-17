#pragma once
#include "gpu/ImageProcessingKernel.h"

namespace ettention
{
    class Image;
    template<typename _T> class GPUMapped;

    class BorderSegmentKernel : public ImageProcessingKernel
    {
    public:
        BorderSegmentKernel(Framework* framework, GPUMapped<Image>* source, GPUMapped<Image>* result);
        BorderSegmentKernel(Framework* framework, GPUMapped<Image>* source);
        ~BorderSegmentKernel();

        void setUsingEightNeighbors(bool useAllEightNeighbors);
        void setInput(GPUMapped<Image>* source);
        GPUMapped<Image>* getInput() const;

    protected:
        void prepareKernelArguments() override;

    private:
        GPUMapped<Image>* source;
        Vec2ui projectionResolution;
        bool useAllEightNeighbors;
    };
}