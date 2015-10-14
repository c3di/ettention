#pragma once

#include <clAmdFft.h>
#include "gpu/ComputeKernel.h"

namespace ettention
{
    class FFTKernel
    {
    public:
        FFTKernel(CLAbstractionLayer* abstractionLayer, Vec2ui inputResolution);
        virtual ~FFTKernel();

        void SetResolution(Vec2ui inputResolution);

    protected:
        void initializeKernel();
        void initializeBuffers();
        void returnBuffers();
        void callToFftLibrary(clAmdFftStatus status);

    protected:
        CLAbstractionLayer* clAbstractionLayer;
        Vec2ui baseResolution;
        cl_command_queue queue;
        clAmdFftPlanHandle kernelPlan;

    private:
        void initializeLibraryIfRequired();
        void closeLibraryIfRequired();

        static unsigned int kernelCount;
        static clAmdFftSetupData* setupData;
    };
}
