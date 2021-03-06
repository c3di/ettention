#pragma once
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationClampingKernel.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationKernel.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationWarpingKernel.h"
#include "algorithm/imagestatistics/globalmaximum/GlobalMaximumPositionImageKernel.h"
#include "framework/math/Vec2.h"
#include "gpu/GPUMapped.h"
#include "gpu/opencl/CLAbstractionLayer.h"

namespace ettention
{
    class Image;

    class CrosscorrelationOperator
    {
    public:
        CrosscorrelationOperator(Framework* framework, GPUMapped<Image>* reference, GPUMapped<Image>* candidate);
        virtual ~CrosscorrelationOperator();

        void setInput(GPUMapped<Image>* reference, GPUMapped<Image>* candidate);
        void matchTemplate();
        void findTranslation();
        void computeCorrelation();
        Vec2f getReferenceImageInfo();
        Vec2f getCandidateImageInfo();
        Vec2ui getPeakPosition();
        float getPeakValue();
        GPUMapped<Image>* getOutput() const;

    protected:
        void precomputeMeansAndDeviations(GPUMapped<Image>* reference, GPUMapped<Image>* candidate);
        void precomputeDeviations(GPUMapped<Image>* reference, GPUMapped<Image>* candidate);
        void precomputeMeans(GPUMapped<Image>* reference, GPUMapped<Image>* candidate);

    private:
        Framework* framework;
        CrosscorrelationKernel* currentKernel;
        CrosscorrelationClampingKernel* nccClampingKernel;
        CrosscorrelationWarpingKernel* nccWarpingKernel;
        GlobalMaximumPositionImageKernel* maxKernel;
        Vec2f means;
        Vec2f deviations;
    };
}