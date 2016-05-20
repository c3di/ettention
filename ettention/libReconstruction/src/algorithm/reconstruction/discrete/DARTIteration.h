#pragma once

#include "algorithm/volumemanipulation/VolumeThresholdOperator.h"
#include "algorithm/volumemanipulation/VolumeBorderSegmentKernel.h"
#include "algorithm/volumemanipulation/VolumeConvolutionOperator.h"
#include "algorithm/reconstruction/discrete/DiscreteReconstructionIterationImplementation.h"
#include "io/serializer/VolumeSerializer.h"

namespace ettention
{
    class GPUMappedVolume;

    class DARTIteration : public DiscreteReconstructionIterationImplementation
    {
    public:
        DARTIteration(Framework* framework, ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod, GPUMappedVolume* source, GPUMappedVolume* initialMask);
        virtual ~DARTIteration();

        virtual void run() override;

        virtual bool didReachStopCriterion() override;

        virtual GPUMappedVolume* getOutput() const override;

        virtual void setInput(GPUMappedVolume* volume);

        void dumpIntermediateVolumes(const std::string& pathPrefix) const;

    private:
        GPUMappedVolume* source;
        GPUMappedVolume* maskVolume;
        GPUMappedVolume* smoothedBoundary;
        GPUMappedVolume* gaussianBlurConvolutionKernel;

        VolumeThresholdOperator* thresholdKernel;
        VolumeBorderSegmentKernel* borderSegmentKernel;
        VolumeConvolutionOperator* gaussianBlurOperator;

        int voxelsAffectedByLastIteration;
        double CONVERGENCE_THRESHOLD{0.00001};
    };

}