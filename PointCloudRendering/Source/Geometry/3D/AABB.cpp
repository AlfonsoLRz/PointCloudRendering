#include "stdafx.h"
#include "AABB.h"

/// [Public methods]

AABB::AABB(const vec3& min, const vec3& max) :
	_max(max), _min(min)
{
}

AABB::AABB(const AABB& aabb) :
	_max (aabb._max), _min(aabb._min)
{
}

AABB::~AABB()
{
}

AABB& AABB::operator=(const AABB& aabb)
{
	_max = aabb._max;
	_min = aabb._min;

	return *this;
}

std::vector<AABB> AABB::split(const unsigned edgeDivisions) const
{
	std::vector<AABB> aabb;
	const float xSize = (_max.x - _min.x) / edgeDivisions, ySize = (_max.y - _min.y) / edgeDivisions, zSize = (_max.z - _min.z) / edgeDivisions;

	for (unsigned x = 0; x < edgeDivisions; ++x)
	{
		for (unsigned y = 0; y < edgeDivisions; ++y)
		{
			for (unsigned z = 0; z < edgeDivisions; ++z)
			{
				const vec3 min(_min.x + xSize * x, _min.y + ySize * y, _min.z + zSize * z);
				const vec3 max(_min.x + xSize * (x + 1), _min.y + ySize * (y + 1), _min.z + zSize * (z + 1));

				aabb.push_back(AABB(min, max));
			}
		}
	}

	return aabb;
}

void AABB::update(const AABB& aabb)
{
	this->update(aabb.max());
	this->update(aabb.min());
}

void AABB::update(const vec3& point)
{
	if (point.x < _min.x) { _min.x = point.x; }
	if (point.y < _min.y) { _min.y = point.y; }
	if (point.z < _min.z) { _min.z = point.z; }

	if (point.x > _max.x) { _max.x = point.x; }
	if (point.y > _max.y) { _max.y = point.y; }
	if (point.z > _max.z) { _max.z = point.z; }
}
