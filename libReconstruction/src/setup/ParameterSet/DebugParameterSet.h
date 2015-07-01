#pragma once
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class DebugParameterSet : public ParameterSet
    {
    public:
        enum ProfilingLevel
        {
            PROFILING_NONE = 0,
            PROFILING_OVERALL = 1,
            PROFILING_NORMAL = 2,
            PROFILING_DETAIL = 3,
        };

    private:
        bool writeProjections;
        bool writeProjectionsInFourierSpace;
        bool writeVirtualProjections;
        bool writeVirtualProjectionsInFourierSpace;
        bool writeResiduals;
        bool writeResidualsInFourierSpace;
        bool writeRayLengthImages;
        bool writeIntermediateVolumes;
        bool writeIntermediateVolumesAfterIterations;
        bool printProgressInfo;
        bool displayKernelParameters;
        bool displayProjectionRMS;
        boost::filesystem::path debugInfoPath;
        ProfilingLevel profilingLevel;
        boost::filesystem::path performanceReportFile;

        ProfilingLevel ParseProfilingLevel(const std::string& profilingString) const;

    public:
        DebugParameterSet(const ParameterSource* parameterSource);
        ~DebugParameterSet();

        bool WriteProjections() const;
        bool WriteProjectionsInFourierSpace() const;
        bool WriteVirtualProjections() const;
        bool WriteVirtualProjectionsInFourierSpace() const;
        bool WriteResiduals() const;
        bool WriteRayLengthImages() const;
        bool WriteIntermediateVolumes() const;
        bool WriteIntermediateVolumesAfterIterations() const;
        bool PrintProgressInfo() const;
        bool DisplayKernelParameters() const;
        bool DisplayProjectionRMS() const;
        std::string PerformanceReportFile() const;
        std::string DebugInfoPath() const;
        ProfilingLevel GetProfilingLevel() const;
    };
}