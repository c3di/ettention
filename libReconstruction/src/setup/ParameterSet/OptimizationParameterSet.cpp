#include "stdafx.h"
#include "OptimizationParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    OptimizationParameterSet::OptimizationParameterSet(const ParameterSource* parameterSource)
    {
        voxelRepresentation = VoxelType::FLOAT_32;
        if(parameterSource->parameterExists("voxelRepresentation"))
        {
            voxelRepresentation = VoxelTypeInformation::parseVoxelType(parameterSource->getStringParameter("voxelRepresentation"));
        }
    }

    OptimizationParameterSet::OptimizationParameterSet(VoxelType voxelRepresentation)
        : voxelRepresentation(voxelRepresentation)
    {
    }

    OptimizationParameterSet::~OptimizationParameterSet()
    {
    }

    VoxelType OptimizationParameterSet::VoxelRepresentation() const
    {
        return voxelRepresentation;
    }
}