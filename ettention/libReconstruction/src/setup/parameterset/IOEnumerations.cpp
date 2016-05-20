#include "stdafx.h"
#include "IOEnumerations.h"
#include "framework/error/Exception.h"

namespace ettention
{
    CoordinateOrder parseVoxelCoordinateOrder(const std::string& coordinateOrder)
    {
        if(coordinateOrder == "xzy_order")
        {
            return ORDER_XZY;
        }
        else if(coordinateOrder == "xyz_order")
        {
            return ORDER_XYZ;
        }
        else if(coordinateOrder == "yxz_order")
        {
            return ORDER_YXZ;
        }
        else if(coordinateOrder == "yzx_order")
        {
            return ORDER_YZX;
        }
        else if(coordinateOrder == "zxy_order")
        {
            return ORDER_ZXY;
        }
        else if(coordinateOrder == "zyx_order")
        {
            return ORDER_ZYX;
        }
        else
        {
            throw Exception("Unknown coordinate order: " + coordinateOrder + "!");
        }
    }

    VoxelValueType parseVoxelValueType(const std::string& voxelValueType)
    {
        if(voxelValueType == "unsigned8")
        {
            return IO_VOXEL_TYPE_UNSIGNED_8;
        }
        else if(voxelValueType == "unsigned16")
        {
            return IO_VOXEL_TYPE_UNSIGNED_16;
        }
        else if(voxelValueType == "float32")
        {
            return IO_VOXEL_TYPE_FLOAT_32;
        }
        else if(voxelValueType == "signed16")
        {
            return IO_VOXEL_TYPE_SIGNED_16;
        }
        else
        {
            throw Exception("ParseVoxelValueType: Unknown file mode ");
        }
    }

    size_t getVoxelValueSizeInBytes(VoxelValueType type)
    {
        switch( type )
        {
        case IO_VOXEL_TYPE_FLOAT_32:
            return sizeof(float);
        case IO_VOXEL_TYPE_UNSIGNED_8:
            return sizeof(uint8_t);
        case IO_VOXEL_TYPE_UNSIGNED_16:
            return sizeof(uint16_t);
        case IO_VOXEL_TYPE_SIGNED_16:
            return sizeof(int16_t);
        }
        throw Exception("GetVoxelValueSizeInBytes: wrong voxel type ");
    }

    void fillOrderDependentIndices(unsigned int orderDependentIndices[3], CoordinateOrder order)
    {
        switch( order )
        {
        case ORDER_XYZ:
            orderDependentIndices[0] = 2;
            orderDependentIndices[1] = 1;
            orderDependentIndices[2] = 0;
            break;
        case ORDER_XZY:
            orderDependentIndices[0] = 1;
            orderDependentIndices[1] = 2;
            orderDependentIndices[2] = 0;
            break;
        case ORDER_YXZ:
            orderDependentIndices[0] = 2;
            orderDependentIndices[1] = 0;
            orderDependentIndices[2] = 1;
            break;
        case ORDER_YZX:
            orderDependentIndices[0] = 0;
            orderDependentIndices[1] = 2;
            orderDependentIndices[2] = 1;
            break;
        case ORDER_ZXY:
            orderDependentIndices[0] = 1;
            orderDependentIndices[1] = 0;
            orderDependentIndices[2] = 2;
            break;
        case ORDER_ZYX:
            orderDependentIndices[0] = 0;
            orderDependentIndices[1] = 1;
            orderDependentIndices[2] = 2;
            break;
        }
    }

    ettention::Vec3ui getOrderDependentResolution(Vec3ui resolution, CoordinateOrder order)
    {
        unsigned int orderDependentIndices[3];
        fillOrderDependentIndices(orderDependentIndices, order);

        Vec3ui orderDependentResolution;
        orderDependentResolution.z = resolution[orderDependentIndices[0]];
        orderDependentResolution.y = resolution[orderDependentIndices[1]];
        orderDependentResolution.x = resolution[orderDependentIndices[2]];
        return orderDependentResolution;
    }

    std::ostream& operator<<(std::ostream& out, VoxelValueType voxelValueType)
    {
        switch(voxelValueType)
        {
        case IO_VOXEL_TYPE_FLOAT_32:
            out << "gray scale (32 bit float)";
            break;
        case IO_VOXEL_TYPE_UNSIGNED_8:
            out << "gray scale (8 bit unsigned)";
            break;
        case IO_VOXEL_TYPE_UNSIGNED_16:
            out << "gray scale (16 bit unsigned)";
            break;
        case IO_VOXEL_TYPE_SIGNED_16:
            out << "gray scale (16 bit signed)";
            break;
        }

        return out;
    }
}