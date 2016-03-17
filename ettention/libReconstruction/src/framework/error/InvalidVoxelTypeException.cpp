#include "stdafx.h"
#include "InvalidVoxelTypeException.h"

namespace ettention
{
    InvalidVoxelTypeException::InvalidVoxelTypeException()
        : Exception("Invalid voxel type")
    {
    }

    InvalidVoxelTypeException::~InvalidVoxelTypeException()
    {
    }
}