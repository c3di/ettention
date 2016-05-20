#include "stdafx.h"
#include "BlockIterativeReconstructionAlgorithmLogger.h"
#include "framework/Logger.h"
#include "framework/Framework.h"
#include "framework/time/PerformanceReport.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeState.h"
#include "algorithm/reconstruction/blockiterative/BlockIterativeProjectionAccess.h"
#include "setup/parameterset/OutputParameterSet.h"

namespace ettention
{
    BlockIterativeReconstructionAlgorithmLogger::BlockIterativeReconstructionAlgorithmLogger()
        : projectionFftKernel(nullptr)
        , rmsKernel(nullptr)
    {

    }

    BlockIterativeReconstructionAlgorithmLogger::~BlockIterativeReconstructionAlgorithmLogger()
    {
        delete rmsKernel;
        delete projectionFftKernel;
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnReconstructionStart(Framework* framework,
                                                                            BlockIterativeProjectionAccess* scheme,
                                                                            BlockIterativeState* data,
                                                                            GPUMappedVolume* volume,
                                                                            unsigned int numberOfIterations,
                                                                            bool longObjectCompensation)
    {
        this->framework = framework;
        this->debugParameterSet = framework->getParameterSet()->get<DebugParameterSet>();
        this->scheme = scheme;
        this->data = data;
        this->numberOfItearions = numberOfItearions;
        this->volume = volume;
        if( debugParameterSet->getProfilingLevel() >= DebugParameterSet::ProfilingLevel::OVERALL )
        {
            overallTimer.start();
        }
        if(debugParameterSet->shouldWriteProjectionsInFourierSpace())
        {
            projectionFftKernel = new FFTForwardKernel(framework->getOpenCLStack(), nullptr);
        }
        if(debugParameterSet->shouldDisplayProjectionRMS())
        {
            rmsKernel = new RMSKernel(framework, nullptr);
        }
        progress = 0.0f;
        progressDelta = 1.0f / (float)(numberOfIterations * scheme->GetNumberOfProjections() * (2 * volume->getProperties().getSubVolumeCount() + (longObjectCompensation ? 2 : 1)));
        setConsoleTitle("Reconstruction in progress...");
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnReconstructionEnd()
    {
        if( debugParameterSet->getProfilingLevel() >= DebugParameterSet::ProfilingLevel::OVERALL )
        {
            overallTimer.stop();
            LOGGER("Total time spent for reconstruction: " << overallTimer.getElapsedTimeInSec() << "s.");
            setConsoleTitle( (boost::format("Reconstruction finished in %1%min. Saving volume on disk.") % (overallTimer.getElapsedTimeInSec() / 60)).str() );
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnIterationStart(unsigned int iterationIndex)
    {
        currentIteration = iterationIndex;
        if( debugParameterSet->getProfilingLevel() >= DebugParameterSet::ProfilingLevel::NORMAL )
        {
            iterationTimer.start();
            if(framework->getPerformanceReport())
            {
                framework->getPerformanceReport()->setCurrentAttribute("Iteration", iterationIndex);
            }
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnIterationEnd()
    {
        if( debugParameterSet->getProfilingLevel() >= DebugParameterSet::ProfilingLevel::NORMAL )
        {
            iterationTimer.stop();
            LOGGER("Time spent for iteration " << currentIteration << ": " << iterationTimer.getElapsedTimeInSec() << "s.");
        }
        if(debugParameterSet->shouldWriteIntermediateVolumesAfterIterations())
        {
            std::string filename = (boost::format("%s/intermediate_i%03d.mrc") % debugParameterSet->getDebugInfoPath() % currentIteration).str();
            this->WriteVolume(filename);
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnBlockStart()
    {
        if(framework->getPerformanceReport())
        {
            framework->getPerformanceReport()->setCurrentAttribute("Block", data->getCurrentBlockIndex());
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnBlockEnd()
    {
        if(debugParameterSet->shouldWriteIntermediateVolumes())
        {
            std::string filename = (boost::format("%s/intermediate_i%03d_b%03d.mrc") % debugParameterSet->getDebugInfoPath() % currentIteration % data->getCurrentBlockIndex()).str();
            this->WriteVolume(filename);
        }
        if(framework->getPerformanceReport())
        {
            framework->getPerformanceReport()->clearCurrentAttribute("Block");
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnForwardProjectionStart(unsigned int subVolume)
    {
        this->OnKernelStart("FP", subVolume);
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnForwardProjectionEnd()
    {
        this->WriteIfRequested("");
        this->OnKernelEnd();
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnBackProjectionStart(unsigned int subVolume)
    {
        this->OnKernelStart("BP", subVolume);
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnBackProjectionEnd()
    {
        this->OnKernelEnd();
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnKernelStart(const std::string& type, unsigned int subVolume)
    {
        if(framework->getPerformanceReport())
        {
            framework->getPerformanceReport()->setCurrentAttribute("Subvolume", subVolume);
            framework->getPerformanceReport()->setCurrentAttribute("Physical projection index", data->getCurrentPhysicalProjectionIndex());
            framework->getPerformanceReport()->setCurrentAttribute("Projection index", data->getCurrentProjectionIndex());
        }
        if(debugParameterSet->shouldPrintProgressInfo())
        {
            std::stringstream subVolumeStr;
            if(subVolume != NO_SUBVOLUMES)
            {
                subVolumeStr << "subvol. " << (subVolume + 1) << "/" << volume->getProperties().getSubVolumeCount() << ", ";
            }
            LOGGER("Iter. " << (currentIteration + 1) << "/" << numberOfItearions << ", " <<
                   "block " << (data->getCurrentBlockIndex() + 1) << "/" << scheme->GetBlockCount() << ", " <<
                   subVolumeStr.str() <<
                   type << " " << (data->getCurrentProjectionIndex() + 1) << "/" << data->getCurrentBlockSize() << " (physical index: " << data->getCurrentPhysicalProjectionIndex() << ")");
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnKernelEnd()
    {
        this->OnProgress();
        if(framework->getPerformanceReport())
        {
            framework->getPerformanceReport()->clearCurrentAttribute("Subvolume");
            framework->getPerformanceReport()->clearCurrentAttribute("Physical projection index");
            framework->getPerformanceReport()->clearCurrentAttribute("Projection index");
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::WriteVolume(const std::string filename)
    {
        volume->ensureIsUpToDateOnCPU();
        MRCWriter writer;
        const OutputParameterSet format;
        writer.write(volume->getObjectOnCPU(), filename, &format);
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnLongObjectCompensationStart()
    {
        this->OnKernelStart("LOC", NO_SUBVOLUMES);        
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnLongObjectCompensationEnd()
    {
        this->OnKernelEnd();
        this->WriteIfRequested("_after_loc");
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnResidualComputationStart()
    {
        this->OnKernelStart("Residual", NO_SUBVOLUMES);
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnResidualComputationEnd()
    {
        this->OnKernelEnd();
        if(currentIteration == 0 && debugParameterSet->shouldWriteProjections())
        {
            auto img = data->getCurrentRealProjectionImage();
            std::string filename = (boost::format("%s/projection_%03d") % debugParameterSet->getDebugInfoPath() % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), img, ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
        if(debugParameterSet->shouldWriteResiduals())
        {
            std::string filename = (boost::format("%s/residuals_i%03d_%03d") % debugParameterSet->getDebugInfoPath() % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), data->getResidual(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
        if(debugParameterSet->shouldDisplayProjectionRMS())
        {
            rmsKernel->setInput(data->getResidual());
            rmsKernel->run();
            LOGGER("RMS, iteration: " << currentIteration << ", block: " << data->getCurrentBlockIndex() << ", projection index: " << data->getCurrentProjectionIndex() << ", projection: " << scheme->getPhysicalProjectionIndex(data->getCurrentBlockIndex(), data->getCurrentProjectionIndex()) << " = " << rmsKernel->getRootMeanSquare());
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::WriteIfRequested(const std::string& filenameModifier)
    {
        if(debugParameterSet->shouldWriteVirtualProjections())
        {
            std::string filename = (boost::format("%s/virtual_projection%s_i%03d_p%03d") % debugParameterSet->getDebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), data->getVirtualProjection(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
            filename = (boost::format("%s/traversal_length%s_i%03d_p%03d") % debugParameterSet->getDebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), data->getTraversalLength(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
        if(debugParameterSet->shouldWriteVirtualProjectionsInFourierSpace())
        {
            projectionFftKernel->setInput(data->getVirtualProjection());
            projectionFftKernel->run();
            std::string filename = (boost::format("%s/virtual_projection%s_fft_real_i%03d_%03d") % debugParameterSet->getDebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), projectionFftKernel->getOutputRealPart(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
            filename = (boost::format("%s/virtual_projection%s_fft_imaginary_i%03d_%03d") % debugParameterSet->getDebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), projectionFftKernel->getOutputImaginaryPart(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnProgress()
    {
        progress += progressDelta;
        auto secs = overallTimer.getElapsedTimeInSec();
        int eta = (int)(secs / progress * (1.0 - progress));
        std::stringstream title;
        title << "Reconstruction in progress... (" << std::fixed << std::setprecision(2) << (100.0 * progress) << "%)";
        if(progress >= 0.05)
        {
            title << ", eta: ";
            if(eta < 60)
            {
                title << "< 1m";
            }
            else if(eta < 3600)
            {
                title << "~" << (eta / 60) << "m.";
            }
            else
            {
                title << "~" << (eta / 3600) << "h " << ((eta - 3600 * (eta / 3600)) / 60) << "m.";
            }
        }
        else
        {
            title << ". Awaiting 5% for better estimates.";
        }
        setConsoleTitle(title.str());
    }
}
