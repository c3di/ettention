#include "stdafx.h"
#include "PriorKnowledgeParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    PriorKnowledgeParameterSet::PriorKnowledgeParameterSet()
        : volumeMinimumIntensity(boost::none)
        , volumeMaximumIntensity(boost::none)
    {
    }

    PriorKnowledgeParameterSet::PriorKnowledgeParameterSet(const ParameterSource* parameterSource)
    {
        volumeMinimumIntensity = boost::none;
        if(parameterSource->parameterExists("priorknowledge.volumeMinimumIntensity"))
        {
            volumeMinimumIntensity = parameterSource->getFloatParameter("priorknowledge.volumeMinimumIntensity");
        }
        volumeMaximumIntensity = boost::none;
        if(parameterSource->parameterExists("priorknowledge.volumeMaximumIntensity"))
        {
            volumeMaximumIntensity = parameterSource->getFloatParameter("priorknowledge.volumeMaximumIntensity");
        }
        if(parameterSource->parameterExists("priorknowledge.maskVolumeFilename"))
        {
            maskVolumeFilename = parameterSource->getPathParameter("priorknowledge.maskVolumeFilename");
        }
    }

    PriorKnowledgeParameterSet::~PriorKnowledgeParameterSet()
    {
    }

    boost::optional<float> PriorKnowledgeParameterSet::getVolumeMinimumIntensity() const
    {
        return volumeMinimumIntensity;
    }

    boost::optional<float> PriorKnowledgeParameterSet::getVolumeMaximumIntensity() const
    {
        return volumeMaximumIntensity;
    }

    const boost::filesystem::path& PriorKnowledgeParameterSet::getMaskVolumeFilename() const
    {
        return maskVolumeFilename;
    }
}