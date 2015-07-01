#pragma once
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationClampingKernel.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationKernel.h"
#include "algorithm/imagemanipulation/crosscorrelation/CrosscorrelationWarpingKernel.h"
#include "algorithm/imagestatistics/DeviationOperator.h"
#include "algorithm/imagestatistics/GlobalMaximumKernel.h"
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
        GPUMapped<Image>* getResult();
        Image* getResultAsImage();

    protected:
        void prepareData(GPUMapped<Image>* reference, GPUMapped<Image>* candidate);

    private:
        Framework* framework;
        CrosscorrelationKernel* currentKernel;
        CrosscorrelationClampingKernel* nccCKernel;
        CrosscorrelationWarpingKernel* nccWKernel;
        GlobalMaximumKernel* maxKernel;
        Vec2f means;
        Vec2f deviations;
    };
}