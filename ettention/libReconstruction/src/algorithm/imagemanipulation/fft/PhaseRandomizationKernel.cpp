#include "stdafx.h"
#include "PhaseRandomizationKernel.h"
#include "PhaseRandomization_bin2c.h"
#include "gpu/opencl/CLKernel.h"
#include "model/image/Image.h"

namespace ettention
{
    PhaseRandomizationKernel::PhaseRandomizationKernel(Framework* framework, GPUMapped<Image>* imageMagnitude, GPUMapped<Image>* imageRandom)
        : ComputeKernel(framework, PhaseRandomization_kernel_SourceCode, "computePhaseRandomizedImage", "PhaseRandomizationKernel")
        , imageMagnitude(imageMagnitude)
        , imageRandom(imageRandom)
    {
        createOutputBuffer();
    }

    PhaseRandomizationKernel::~PhaseRandomizationKernel()
    {
        deleteOutputBuffer();
    }

    void PhaseRandomizationKernel::prepareKernelArguments()
    {
        implementation->setArgument("resolution", imageMagnitude->getResolution());
        implementation->setArgument("imageRandom", imageRandom);
        implementation->setArgument("imageMagnitude", imageMagnitude);
        implementation->setArgument("imagePhaseShifted_real", imagePhaseShiftedReal);
        implementation->setArgument("imagePhaseShifted_imaginary", imagePhaseShiftedImaginary);
    }

    void PhaseRandomizationKernel::preRun()
    {
        implementation->setGlobalWorkSize(imageMagnitude->getResolution());
    }

    void PhaseRandomizationKernel::postRun()
    {
        imagePhaseShiftedReal->markAsChangedOnGPU();
        imagePhaseShiftedImaginary->markAsChangedOnGPU();
    }

    GPUMapped<Image>* PhaseRandomizationKernel::getRealOutput() const
    {
        return imagePhaseShiftedReal;
    }

    GPUMapped<Image>* PhaseRandomizationKernel::getImaginaryOutput() const
    {
        return imagePhaseShiftedImaginary;
    }

    void PhaseRandomizationKernel::createOutputBuffer()
    {
        imagePhaseShiftedReal = new GPUMapped<Image>(abstractionLayer, imageMagnitude->getResolution().xy());
        imagePhaseShiftedImaginary = new GPUMapped<Image>(abstractionLayer, imageMagnitude->getResolution().xy());
    }

    void PhaseRandomizationKernel::deleteOutputBuffer()
    {
        delete imagePhaseShiftedReal;
        delete imagePhaseShiftedImaginary;
    }
}