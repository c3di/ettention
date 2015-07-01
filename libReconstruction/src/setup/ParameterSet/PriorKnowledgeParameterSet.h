#pragma once
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class PriorKnowledgeParameterSet : public ParameterSet
    {
    public:
        PriorKnowledgeParameterSet();
        PriorKnowledgeParameterSet(const ParameterSource* parameterSource);
        ~PriorKnowledgeParameterSet();

        boost::optional<float> getVolumeMinimumIntensity() const;
        boost::optional<float> getVolumeMaximumIntensity() const;
        const boost::filesystem::path& getMaskVolumeFilename() const;

    private:
        boost::optional<float> volumeMinimumIntensity;
        boost::optional<float> volumeMaximumIntensity;
        boost::filesystem::path maskVolumeFilename;
    };
}