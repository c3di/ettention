#include "stdafx.h"
#include "OptimizationParameterSet.h"
#include "setup/parametersource/ParameterSource.h"

namespace ettention
{
    OptimizationParameterSet::OptimizationParameterSet(const ParameterSource* parameterSource)
    {
        voxelRepresentation = Voxel::DataType::FLOAT_32;
        if(parameterSource->parameterExists("optimization.internalVoxelRepresentationType"))
        {
            voxelRepresentation = Voxel::parseVoxelType(parameterSource->getStringParameter("optimization.internalVoxelRepresentationType"));
            if( voxelRepresentation == Voxel::DataType::UCHAR_8 )
            {
                throw Exception("UChar is not supported as a valid internal voxel storage data type.");
            }
        }

        projectionSetType = std::string("random");
        if( parameterSource->parameterExists("optimization.projectionIteration") )
            projectionSetType = parameterSource->getStringParameter("optimization.projectionIteration");
    }

    OptimizationParameterSet::OptimizationParameterSet(Voxel::DataType voxelRepresentation)
        : voxelRepresentation(voxelRepresentation)
    {
    }

    OptimizationParameterSet::~OptimizationParameterSet()
    {
    }

    Voxel::DataType OptimizationParameterSet::getVoxelRepresentation() const
    {
        return voxelRepresentation;
    }

    const std::string& OptimizationParameterSet::getProjectionSetType() const
    {
        return projectionSetType;
    }

}