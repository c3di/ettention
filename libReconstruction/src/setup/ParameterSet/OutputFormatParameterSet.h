#pragma once
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class ParameterSource;

    class OutputFormatParameterSet : public ParameterSet
    {
    public:
        enum VoxelValueType
        {
            UNSIGNED_8 = 0,
            SIGNED_16 = 1,
            FLOAT_32 = 2,
            UNSIGNED_16 = 6
        };

        enum CoordinateOrder
        {
            ORDER_XYZ = 0,
            ORDER_XZY = 1
        };

        OutputFormatParameterSet(VoxelValueType voxelType = FLOAT_32, CoordinateOrder coordinateOrder = CoordinateOrder::ORDER_XYZ, bool invertData = false);
        OutputFormatParameterSet(const ParameterSource* parameterSource);
        virtual ~OutputFormatParameterSet();

        VoxelValueType VoxelType() const;
        bool InvertData() const;
        CoordinateOrder OutputVolumeRotation() const;

        static VoxelValueType parseVoxelType(const std::string modeString);
        static CoordinateOrder parseVolumeRotation(const std::string rotationString);

    private:
        VoxelValueType voxelType;
        bool invertData;
        CoordinateOrder coordinateOrder;
    };

    std::ostream& operator<<(std::ostream& oss, OutputFormatParameterSet::VoxelValueType mode);
}