#include "stdafx.h"
#include "BlockIterativeReconstructionAlgorithmLogger.h"
#include "framework/Logger.h"
#include "framework/Framework.h"
#include "framework/time/PerformanceReport.h"
#include "io/serializer/ImageSerializer.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "setup/ParameterSet/OutputFormatParameterSet.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeState.h"
#include "algorithm/reconstruction/BlockIterative/BlockIterativeProjectionAccess.h"

namespace ettention
{
    BlockIterativeReconstructionAlgorithmLogger::BlockIterativeReconstructionAlgorithmLogger()
        : projectionFftKernel(0)
        , rmsKernel(0)
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
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_OVERALL)
        {
            overallTimer.start();
        }
        if(debugParameterSet->WriteProjectionsInFourierSpace())
        {
            projectionFftKernel = new FFTForwardKernel(framework->getOpenCLStack(), 0);
        }
        if(debugParameterSet->DisplayProjectionRMS())
        {
            rmsKernel = new RMSKernel(framework, 0);
        }
        progress = 0.0f;
        progressDelta = 1.0f / (float)(numberOfIterations * scheme->GetNumberOfProjections() * (2 * volume->Properties().GetSubVolumeCount() + (longObjectCompensation ? 2 : 1)));
#ifdef _WINDOWS
        SetConsoleTitleA("Reconstruction in progress...");
#endif
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnReconstructionEnd()
    {
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_OVERALL)
        {
            overallTimer.stop();
            LOGGER("Total time spent for reconstruction: " << overallTimer.getElapsedTimeInSec() << "s.");
            if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_DETAIL)
            {
                framework->getPerformanceReport()->createCsvReport(debugParameterSet->PerformanceReportFile());
            }
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnIterationStart(unsigned int iterationIndex)
    {
        currentIteration = iterationIndex;
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_NORMAL)
        {
            iterationTimer.start();
            if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_DETAIL)
            {
                framework->getPerformanceReport()->setCurrentAttribute("Iteration", iterationIndex);
            }
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnIterationEnd()
    {
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_NORMAL)
        {
            iterationTimer.stop();
            LOGGER("Time spent for iteration " << currentIteration << ": " << iterationTimer.getElapsedTimeInSec() << "s.");
        }
        if(debugParameterSet->WriteIntermediateVolumesAfterIterations())
        {
            std::string filename = (boost::format("%s/intermediate_i%03d.mrc") % debugParameterSet->DebugInfoPath() % currentIteration).str();
            this->WriteVolume(filename);
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnBlockStart()
    {
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_DETAIL)
        {
            framework->getPerformanceReport()->setCurrentAttribute("Block", data->getCurrentBlockIndex());
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnBlockEnd()
    {
        if(debugParameterSet->WriteIntermediateVolumes())
        {
            std::string filename = (boost::format("%s/intermediate_i%03d_b%03d.mrc") % debugParameterSet->DebugInfoPath() % currentIteration % data->getCurrentBlockIndex()).str();
            this->WriteVolume(filename);
        }
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_DETAIL)
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
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_DETAIL)
        {
            framework->getPerformanceReport()->setCurrentAttribute("SubVolume", subVolume);
            framework->getPerformanceReport()->setCurrentAttribute("Physical projection index", data->getCurrentPhysicalProjectionIndex());
            framework->getPerformanceReport()->setCurrentAttribute("Projection index", data->getCurrentProjectionIndex());
        }
        if(debugParameterSet->PrintProgressInfo())
        {
            std::stringstream subVolumeStr;
            if(subVolume != NO_SUBVOLUMES)
            {
                subVolumeStr << "subvol. " << (subVolume + 1) << "/" << volume->Properties().GetSubVolumeCount() << ", ";
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
        if(debugParameterSet->GetProfilingLevel() >= DebugParameterSet::PROFILING_DETAIL)
        {
            framework->getPerformanceReport()->clearCurrentAttribute("SubVolume");
            framework->getPerformanceReport()->clearCurrentAttribute("Physical projection index");
            framework->getPerformanceReport()->clearCurrentAttribute("Projection index");
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::WriteVolume(const std::string filename)
    {
        volume->ensureIsUpToDateOnCPU();
        MRCWriter writer;
        const OutputFormatParameterSet format(OutputFormatParameterSet::FLOAT_32);
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
        if(currentIteration == 0 && debugParameterSet->WriteProjections())
        {
            auto img = data->getCurrentRealProjectionImage();
            std::string filename = (boost::format("%s/projection_%03d") % debugParameterSet->DebugInfoPath() % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), img, ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
        if(debugParameterSet->WriteResiduals())
        {
            std::string filename = (boost::format("%s/residuals_i%03d_%03d") % debugParameterSet->DebugInfoPath() % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), data->getResidual(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
        if(debugParameterSet->DisplayProjectionRMS())
        {
            rmsKernel->setInput(data->getResidual());
            rmsKernel->run();
            LOGGER("RMS: " << rmsKernel->getRootMeanSquare());
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::WriteIfRequested(const std::string& filenameModifier)
    {
        if(debugParameterSet->WriteVirtualProjections())
        {
            std::string filename = (boost::format("%s/virtual_projection%s_i%03d_p%03d") % debugParameterSet->DebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), data->getVirtualProjection(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
            filename = (boost::format("%s/traversal_length%s_i%03d_p%03d") % debugParameterSet->DebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), data->getTraversalLength(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
        if(debugParameterSet->WriteVirtualProjectionsInFourierSpace())
        {
            projectionFftKernel->setInput(data->getVirtualProjection());
            projectionFftKernel->run();
            std::string filename = (boost::format("%s/virtual_projection%s_fft_real_i%03d_%03d") % debugParameterSet->DebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), projectionFftKernel->getOutputRealPart(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
            filename = (boost::format("%s/virtual_projection%s_fft_imaginary_i%03d_%03d") % debugParameterSet->DebugInfoPath() % filenameModifier % currentIteration % data->getCurrentPhysicalProjectionIndex()).str();
            ImageSerializer::writeImage(filename.c_str(), projectionFftKernel->getOutputImaginaryPart(), ImageSerializer::ImageFormat::TIFF_GRAY_32);
        }
    }

    void BlockIterativeReconstructionAlgorithmLogger::OnProgress()
    {
        progress += progressDelta;
        auto secs = overallTimer.getElapsedTimeInSec();
        int eta = (int)(secs / progress * (1.0 - progress));
        std::stringstream title;
        title << "Reconstruction in progress... (" << std::fixed << std::setprecision(2) << (100.0 * progress) << "%), eta: ";
        if(progress >= 0.05)
        {
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
            title << " awaiting 5%.";
        }
#ifdef _WINDOWS
        SetConsoleTitleA(title.str().c_str());
#endif
    }
}