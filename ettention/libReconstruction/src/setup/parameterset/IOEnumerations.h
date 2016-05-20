#pragma once
#include "framework/math/Vec3.h"

namespace ettention
{
    enum VoxelValueType
    {
        IO_VOXEL_TYPE_UNSPECIFIED = -1,
        IO_VOXEL_TYPE_UNSIGNED_8 = 0,
        IO_VOXEL_TYPE_SIGNED_16 = 1,
        IO_VOXEL_TYPE_FLOAT_32 = 2,
        IO_VOXEL_TYPE_UNSIGNED_16 = 6
    };

    enum CoordinateOrder
    {
        ORDER_XYZ,
        ORDER_XZY,
        ORDER_YXZ,
        ORDER_YZX,
        ORDER_ZXY,
        ORDER_ZYX,
    };

    CoordinateOrder parseVoxelCoordinateOrder(const std::string& coordinateOrder);
    VoxelValueType parseVoxelValueType(const std::string& voxelValueType);
    size_t getVoxelValueSizeInBytes(VoxelValueType type);
    void fillOrderDependentIndices(unsigned int orderDependentIndices[3], CoordinateOrder order);
    Vec3ui getOrderDependentResolution(Vec3ui resolution, CoordinateOrder order);

    std::ostream& operator<<(std::ostream& out, VoxelValueType type);
}