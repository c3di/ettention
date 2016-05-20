#include "stdafx.h"
#include "DARTIteration.h"

#include "framework/Framework.h"
#include "algorithm/volumemanipulation/ConvolutionKernelGenerator.h"
#include "io/serializer/MRCWriter.h"
#include "setup/parameterset/AlgorithmParameterSet.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{

    DARTIteration::DARTIteration(Framework* framework, ReconstructionAlgorithmUsingMask* internalAlgebraicReconstructionMethod, GPUMappedVolume* source, GPUMappedVolume* initialMask)
        : DiscreteReconstructionIterationImplementation(framework, internalAlgebraicReconstructionMethod)
        , source(source)
        , maskVolume(initialMask)
        , voxelsAffectedByLastIteration( static_cast<int>(source->getObjectOnCPU()->getProperties().getVolumeVoxelCount()))
    {
        const AlgorithmParameterSet* params = framework->getParameterSet()->get<AlgorithmParameterSet>();
        auto discreteParams = params->getDiscreteReconstructionParameters();
        auto levels = discreteParams.interval.getThresholdValues();
        auto values = discreteParams.interval.getIntensityValues();

        maskVolume = internalAlgebraicReconstructionMethod->getMaskVolume();
        thresholdKernel = new VolumeThresholdOperator(framework, source, &levels, &values);
        borderSegmentKernel = new VolumeBorderSegmentKernel(framework, thresholdKernel->getOutput(), maskVolume);
        internalAlgebraicReconstructionMethod->setMaskVolume(borderSegmentKernel->getOutput());

        smoothedBoundary = new GPUMappedVolume(framework->getOpenCLStack(), new FloatVolume(source->getProperties().getVolumeResolution(), 0.0f));
        smoothedBoundary->takeOwnershipOfObjectOnCPU();

        gaussianBlurConvolutionKernel = new GPUMappedVolume(framework->getOpenCLStack(), ConvolutionKernelGenerator::generateGaussianBlurConvolutionKernel());
        gaussianBlurConvolutionKernel->takeOwnershipOfObjectOnCPU();

        gaussianBlurOperator = new VolumeConvolutionOperator(  framework
                                                             , internalAlgebraicReconstructionMethod->getReconstructedVolume()
                                                             , smoothedBoundary
                                                             , gaussianBlurConvolutionKernel
                                                             , borderSegmentKernel->getOutput());
    }

    DARTIteration::~DARTIteration()
    {
        delete gaussianBlurOperator;
        delete gaussianBlurConvolutionKernel;
        delete smoothedBoundary;
        delete borderSegmentKernel;
        delete thresholdKernel;
    }

    void DARTIteration::run()
    {
        thresholdKernel->run();
        borderSegmentKernel->run();
        internalAlgebraicReconstructionMethod->run();

        gaussianBlurOperator->run();
        thresholdKernel->setInput(internalAlgebraicReconstructionMethod->getReconstructedVolume());
        //thresholdKernel->setInput(smoothedBoundary);
    }

    bool DARTIteration::didReachStopCriterion()
    {
        borderSegmentKernel->getOutput()->ensureIsUpToDateOnCPU();
        auto* borderVolume = borderSegmentKernel->getOutput()->getObjectOnCPU();

        bool stop = true;
        int nonZeroCount = 0;
        for( size_t i = 0; i < borderVolume->getProperties().getVolumeVoxelCount(); ++i )
        {
            if( borderVolume->getVoxelValue(i) )
            {
                stop = false; // Do not stop if border is not empty
                ++nonZeroCount;
            }
        }

        if(!stop)
        {
            double diff = (double)(abs(nonZeroCount - voxelsAffectedByLastIteration)) / voxelsAffectedByLastIteration;
            voxelsAffectedByLastIteration = nonZeroCount;
            if( diff < CONVERGENCE_THRESHOLD )
                std::cout << "Reached convergence threshold" << std::endl;

            return diff < CONVERGENCE_THRESHOLD;
        }

        return stop;
    }

    void DARTIteration::setInput(GPUMappedVolume* volume)
    {
        this->source = volume;
        thresholdKernel->setInput(volume);
    }

    GPUMappedVolume* DARTIteration::getOutput() const
    {
        thresholdKernel->getOutput()->ensureIsUpToDateOnCPU();
        return thresholdKernel->getOutput();
    }

    void DARTIteration::dumpIntermediateVolumes(const std::string& pathPrefix) const
    {
        MRCWriter writer;
        const OutputParameterSet format(IO_VOXEL_TYPE_FLOAT_32, ORDER_XYZ);
        thresholdKernel->getOutput()->ensureIsUpToDateOnCPU();
        borderSegmentKernel->getOutput()->ensureIsUpToDateOnCPU();
        internalAlgebraicReconstructionMethod->getReconstructedVolume()->ensureIsUpToDateOnCPU();
        smoothedBoundary->ensureIsUpToDateOnCPU();
        writer.write(thresholdKernel->getOutput()->getObjectOnCPU(), pathPrefix + "_1thresh.mrc", &format);
        writer.write(borderSegmentKernel->getOutput()->getObjectOnCPU(), pathPrefix + "_2border.mrc", &format);
        writer.write(internalAlgebraicReconstructionMethod->getReconstructedVolume()->getObjectOnCPU(), pathPrefix + "_3recons.mrc", &format);
        writer.write(smoothedBoundary->getObjectOnCPU(), pathPrefix + "_4smooth.mrc", &format);
    }

}