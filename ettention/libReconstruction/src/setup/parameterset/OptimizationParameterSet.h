#pragma once
#include "model/volume/Voxel.h"
#include "setup/parameterset/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class OptimizationParameterSet : public ParameterSet
    {
    private:
        Voxel::DataType voxelRepresentation;
        std::string projectionSetType;

    public:
        OptimizationParameterSet(const ParameterSource* parameterSource);
        OptimizationParameterSet(Voxel::DataType voxelRepresentation);
        ~OptimizationParameterSet();

        Voxel::DataType getVoxelRepresentation() const;
        const std::string& getProjectionSetType() const;
    };
}