#pragma once

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

    std::ostream& operator<<(std::ostream& out, VoxelValueType type);
}