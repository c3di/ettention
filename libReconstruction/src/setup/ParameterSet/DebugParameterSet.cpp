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

        profilingLevel = PROFILING_OVERALL;
        if(parameterSource->parameterExists("debug.profilingLevel"))
        {
            profilingLevel = ParseProfilingLevel(parameterSource->getStringParameter("debug.profilingLevel"));
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
    }

    DebugParameterSet::~DebugParameterSet()
    {
    }

    std::string DebugParameterSet::DebugInfoPath() const
    {
        return debugInfoPath.string();
    }

    DebugParameterSet::ProfilingLevel DebugParameterSet::ParseProfilingLevel(const std::string& profilingString) const
    {
        if(profilingString == "none")
        {
            return PROFILING_NONE;
        }
        else if(profilingString == "overall")
        {
            return PROFILING_OVERALL;
        }
        else if(profilingString == "normal")
        {
            return PROFILING_NORMAL;
        }
        else if(profilingString == "detail")
        {
            return PROFILING_DETAIL;
        }
        else
        {
            throw Exception("Unknown profiling level constant. Supported is 'overall', 'normal' and 'detail'");
        }
    }

    DebugParameterSet::ProfilingLevel DebugParameterSet::GetProfilingLevel() const
    {
        return profilingLevel;
    }

    bool DebugParameterSet::WriteProjections() const
    {
        return writeProjections;
    }

    bool DebugParameterSet::WriteProjectionsInFourierSpace() const
    {
        return writeProjectionsInFourierSpace;
    }

    bool DebugParameterSet::WriteVirtualProjections() const
    {
        return writeVirtualProjections;
    }

    bool DebugParameterSet::WriteVirtualProjectionsInFourierSpace() const
    {
        return writeVirtualProjectionsInFourierSpace;
    }

    bool DebugParameterSet::WriteResiduals() const
    {
        return writeResiduals;
    }

    bool DebugParameterSet::WriteRayLengthImages() const
    {
        return writeRayLengthImages;
    }

    bool DebugParameterSet::WriteIntermediateVolumes() const
    {
        return writeIntermediateVolumes;
    }

    bool DebugParameterSet::WriteIntermediateVolumesAfterIterations() const
    {
        return writeIntermediateVolumesAfterIterations;
    }

    bool DebugParameterSet::PrintProgressInfo() const
    {
        return printProgressInfo;
    }

    bool DebugParameterSet::DisplayKernelParameters() const
    {
        return displayKernelParameters;
    }

    bool DebugParameterSet::DisplayProjectionRMS() const
    {
        return displayProjectionRMS;
    }

    std::string DebugParameterSet::PerformanceReportFile() const
    {
        return performanceReportFile.string();
    }
}