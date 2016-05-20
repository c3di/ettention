#include "stdafx.h"
#include "DebugParameterSet.h"
#include "framework/error/Exception.h"
#include "framework/time/PerformanceReport.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    DebugParameterSet::DebugParameterSet(const ParameterSource* parameterSource)
    {
        displayKernelParameters = false;
        if(parameterSource->parameterExists("debug.outputKernelParameters"))
        {
            displayKernelParameters = parameterSource->getBoolParameter("debug.outputKernelParameters");
        }

        displayProjectionRMS = false;
        if(parameterSource->parameterExists("debug.displayProjectionRMS"))
        {
            displayProjectionRMS = parameterSource->getBoolParameter("debug.displayProjectionRMS");
        }

        writeProjections = false;
        if(parameterSource->parameterExists("debug.writeProjections"))
        {
            writeProjections = parameterSource->getBoolParameter("debug.writeProjections");
        }

        writeProjectionsInFourierSpace = false;
        if(parameterSource->parameterExists("debug.writeProjectionsInFourierSpace"))
        {
            writeProjectionsInFourierSpace = parameterSource->getBoolParameter("debug.writeProjectionsInFourierSpace");
        }

        writeVirtualProjections = false;
        if(parameterSource->parameterExists("debug.writeVirtualProjections"))
        {
            writeVirtualProjections = parameterSource->getBoolParameter("debug.writeVirtualProjections");
        }

        writeVirtualProjectionsInFourierSpace = false;
        if(parameterSource->parameterExists("debug.writeVirtualProjectionsInFourierSpace"))
        {
            writeVirtualProjectionsInFourierSpace = parameterSource->getBoolParameter("debug.writeVirtualProjectionsInFourierSpace");
        }

        writeResiduals = false;
        if(parameterSource->parameterExists("debug.writeResiduals"))
        {
            writeResiduals = parameterSource->getBoolParameter("debug.writeResiduals");
        }

        writeRayLengthImages = false;
        if(parameterSource->parameterExists("debug.writeRayLength"))
        {
            writeRayLengthImages = parameterSource->getBoolParameter("debug.writeRayLength");
        }

        writeIntermediateVolumes = false;
        if(parameterSource->parameterExists("debug.writeIntermediateVolumes"))
        {
            writeIntermediateVolumes = parameterSource->getBoolParameter("debug.writeIntermediateVolumes");
        }

        writeIntermediateVolumesAfterIterations = false;
        if(parameterSource->parameterExists("debug.writeIntermediateVolumesAfterIterations"))
        {
            writeIntermediateVolumesAfterIterations = parameterSource->getBoolParameter("debug.writeIntermediateVolumesAfterIterations");
        }

        printProgressInfo = false;
        if(parameterSource->parameterExists("debug.logProgressInfo"))
        {
            printProgressInfo = parameterSource->getBoolParameter("debug.logProgressInfo");
        }

        performanceReportFile = boost::filesystem::path("performance.csv");
        if(parameterSource->parameterExists("debug.performanceReportFile"))
        {
            performanceReportFile = parameterSource->getPathParameter("debug.performanceReportFile");
        }

        profilingLevel = ProfilingLevel::OVERALL;
        if(parameterSource->parameterExists("debug.profilingLevel"))
        {
            profilingLevel = doParseProfilingLevel(parameterSource->getStringParameter("debug.profilingLevel"));
        }

        debugInfoPath = boost::filesystem::path("debug");
        if(parameterSource->parameterExists("debug.infoPath"))
        {
            debugInfoPath = parameterSource->getPathParameter("debug.infoPath");
        }
        if(!boost::filesystem::is_directory(debugInfoPath))
        {
            boost::filesystem::create_directories(debugInfoPath);
        }

        if(parameterSource->parameterExists("debug.meshExportPath"))
        {
            meshExportPath = parameterSource->getPathParameter("debug.meshExportPath");
        }
        meshExportProjectionStepRate = 1;
        if(parameterSource->parameterExists("debug.meshExportProjectionStepRate"))
        {
            meshExportProjectionStepRate = parameterSource->getUIntParameter("debug.meshExportProjectionStepRate");
        }
    }

    DebugParameterSet::~DebugParameterSet()
    {
    }

    std::string DebugParameterSet::getDebugInfoPath() const
    {
        return debugInfoPath.string();
    }

    DebugParameterSet::ProfilingLevel DebugParameterSet::doParseProfilingLevel(const std::string& profilingString) const
    {
        if(profilingString == "none")
        {
            return ProfilingLevel::NONE;
        } else
        if(profilingString == "overall")
        {
            return ProfilingLevel::OVERALL;
        } else
        if(profilingString == "normal")
        {
            return ProfilingLevel::NORMAL;
        } else
        if(profilingString == "detail")
        {
            return ProfilingLevel::DETAIL;
        } else
        if( profilingString == "timing" )
        {
            return ProfilingLevel::TRANSFER_TIMING;
        } else {
            throw Exception("Unknown profiling level constant. Supported is 'overall', 'normal', 'detail' and 'timing'");
        }
    }

    DebugParameterSet::ProfilingLevel DebugParameterSet::getProfilingLevel() const
    {
        return profilingLevel;
    }

    bool DebugParameterSet::shouldWriteProjections() const
    {
        return writeProjections;
    }

    bool DebugParameterSet::shouldWriteProjectionsInFourierSpace() const
    {
        return writeProjectionsInFourierSpace;
    }

    bool DebugParameterSet::shouldWriteVirtualProjections() const
    {
        return writeVirtualProjections;
    }

    bool DebugParameterSet::shouldWriteVirtualProjectionsInFourierSpace() const
    {
        return writeVirtualProjectionsInFourierSpace;
    }

    bool DebugParameterSet::shouldWriteResiduals() const
    {
        return writeResiduals;
    }

    bool DebugParameterSet::shouldWriteRayLengthImages() const
    {
        return writeRayLengthImages;
    }

    bool DebugParameterSet::shouldWriteIntermediateVolumes() const
    {
        return writeIntermediateVolumes;
    }

    bool DebugParameterSet::shouldWriteIntermediateVolumesAfterIterations() const
    {
        return writeIntermediateVolumesAfterIterations;
    }

    bool DebugParameterSet::shouldPrintProgressInfo() const
    {
        return printProgressInfo;
    }

    bool DebugParameterSet::shouldDisplayKernelParameters() const
    {
        return displayKernelParameters;
    }

    bool DebugParameterSet::shouldDisplayProjectionRMS() const
    {
        return displayProjectionRMS;
    }

    std::string DebugParameterSet::getPerformanceReportFile() const
    {
        return performanceReportFile.string();
    }

    const boost::optional<boost::filesystem::path>& DebugParameterSet::getMeshExportPath() const
    {
        return meshExportPath;
    }

    unsigned int DebugParameterSet::getMeshExportProjectionStepRate() const
    {
        return meshExportProjectionStepRate;
    }
}