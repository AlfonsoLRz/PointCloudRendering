#include "stdafx.h"
#include "Interpolation.h"


Interpolation::Interpolation(std::vector<vec4>& waypoints) : _waypoints(waypoints)
{
}

Interpolation::~Interpolation()
{
}

float Interpolation::getLineLength(std::vector<vec4>& points)
{
	float length = 0.0f;

	for (unsigned i = 1; i < points.size(); ++i)
	{
		length += glm::distance(points[i], points[i - 1]);
	}

	return length;
}