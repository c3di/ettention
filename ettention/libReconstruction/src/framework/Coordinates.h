#pragma once

#include "math/Vec2.h"
#include "math/Vec3.h"

namespace ettention
{
	unsigned int Flatten3D(Vec3ui index, Vec3ui size)
	{
		return index.z * size.y * size.x + index.y * size.x + index.x;
	}

	Vec3ui Unflatten3D(unsigned int index, Vec3ui size)
	{
		return Vec3ui(index % size.x, (index / size.x) % size.y, (index / (size.x*size.y)) % size.z);
	}

	unsigned int Flatten2D(Vec2ui index, Vec2ui size)
	{
		return index.y * size.x + index.x;
	}

}