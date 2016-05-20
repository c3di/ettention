#pragma once
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class DebugParameterSet : public ParameterSet
    {
    public:
        enum class ProfilingLevel
        {
            NONE = 0,
            OVERALL = 1,
            NORMAL = 2,
            DETAIL = 3,
            TRANSFER_TIMING = 4
        };

        DebugParameterSet(const ParameterSource* parameterSource);
        ~DebugParameterSet();

        bool shouldWriteProjections() const;
        bool shouldWriteProjectionsInFourierSpace() const;
        bool shouldWriteVirtualProjections() const;
        bool shouldWriteVirtualProjectionsInFourierSpace() const;
        bool shouldWriteResiduals() const;
        bool shouldWriteRayLengthImages() const;
        bool shouldWriteIntermediateVolumes() const;
        bool shouldWriteIntermediateVolumesAfterIterations() const;
        bool shouldPrintProgressInfo() const;
        bool shouldDisplayKernelParameters() const;
        bool shouldDisplayProjectionRMS() const;
        std::string getPerformanceReportFile() const;
        std::string getDebugInfoPath() const;
        ProfilingLevel getProfilingLevel() const;
        const boost::optional<boost::filesystem::path>& getMeshExportPath() const;
        unsigned int getMeshExportProjectionStepRate() const;

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
        boost::optional<boost::filesystem::path> meshExportPath;
        unsigned int meshExportProjectionStepRate;

        ProfilingLevel doParseProfilingLevel(const std::string& profilingString) const;
    };
}