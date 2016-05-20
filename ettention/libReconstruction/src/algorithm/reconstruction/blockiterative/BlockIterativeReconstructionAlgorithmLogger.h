#pragma once
#include "framework/ProgressLogger.h"
#include "algorithm/imagemanipulation/fft/FFTForwardKernel.h"
#include "algorithm/imagemanipulation/RMSKernel.h"
#include "framework/time/Timer.h"
#include "model/projectionset/ProjectionSet.h"
#include "model/volume/GPUMappedVolume.h"
#include "setup/parameterset/DebugParameterSet.h"

namespace ettention
{
    class BlockIterativeProjectionAccess;
    class BlockIterativeState;
    class CLAbstractionLayer;

    class BlockIterativeReconstructionAlgorithmLogger : public ProgressLogger
    {
    protected:
        static const unsigned int NO_SUBVOLUMES = (unsigned int)-1;

        BlockIterativeProjectionAccess* scheme;
        BlockIterativeState* data;
        const DebugParameterSet* debugParameterSet;
        Framework* framework;
        GPUMappedVolume* volume;
        Timer overallTimer;
        Timer iterationTimer;
        unsigned int currentIteration;
        unsigned int numberOfItearions;
        FFTForwardKernel* projectionFftKernel;
        RMSKernel* rmsKernel;

        float progress;
        float progressDelta;

        void WriteVolume(const std::string filename);
        void WriteIfRequested(const std::string& filenameModifier);
        void OnKernelStart(const std::string& type, unsigned int subVolume);
        void OnKernelEnd();
        void OnProgress();

    public:
        BlockIterativeReconstructionAlgorithmLogger();
        ~BlockIterativeReconstructionAlgorithmLogger();

        void OnReconstructionStart(Framework* framework,
                                   BlockIterativeProjectionAccess* scheme,
                                   BlockIterativeState* data,
                                   GPUMappedVolume* volume,
                                   unsigned int numberOfIterations,
                                   bool longObjectCompensation);

        void OnIterationStart(unsigned int iterationIndex);
        void OnBlockStart();

        void OnForwardProjectionStart(unsigned int subVolume);
        void OnForwardProjectionEnd();
        void OnLongObjectCompensationStart();
        void OnLongObjectCompensationEnd();
        void OnResidualComputationStart();
        void OnResidualComputationEnd();
        void OnBackProjectionStart(unsigned int subVolume);
        void OnBackProjectionEnd();

        void OnBlockEnd();
        void OnIterationEnd();
        void OnReconstructionEnd();
    };
}