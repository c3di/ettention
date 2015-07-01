#pragma once
#include "model/volume/VoxelType.h"
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class OptimizationParameterSet : public ParameterSet
    {
    private:
        VoxelType voxelRepresentation;

    public:
        OptimizationParameterSet(const ParameterSource* parameterSource);
        OptimizationParameterSet(VoxelType voxelRepresentation);
        ~OptimizationParameterSet();

        enum VoxelType VoxelRepresentation() const;
    };
}