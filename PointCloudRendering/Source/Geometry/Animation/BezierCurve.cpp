#include "stdafx.h"
#include "BezierCurve.h"

#include "Graphics/Core/ShaderList.h"

/// [Public methods]

BezierCurve::BezierCurve(std::vector<vec4>& waypoints)
	: Interpolation(waypoints)
{
}

BezierCurve::~BezierCurve()
{
}

vec4 BezierCurve::getPosition(float t, bool& finished)
{
	uvec4 indices;

	t = glm::clamp(t, .0f, 1.0f) * (_waypoints.size() - 1) / 3.0f;
	indices.x = unsigned(t);
	t -= indices.x;
	indices.x *= 3;
	indices.y = indices.x + 1;
	indices.z = indices.y + 1;
	indices.w = indices.z + 1;
	indices = glm::clamp(indices, unsigned(0), unsigned(_waypoints.size() - 1));

	finished = indices.x == (_waypoints.size() - 4);
	
	return getPoint(indices.x, indices.y, indices.z, indices.w, t);
}

/// [Protected methods]

vec4 BezierCurve::getPoint(const unsigned index1, const unsigned index2, const unsigned index3, const unsigned index4, const float t)
{
	float oneMinusT = 1.0f - t;

	return oneMinusT * oneMinusT * oneMinusT * _waypoints[index1] + 3.0f * oneMinusT * oneMinusT * t * _waypoints[index2] + 
		   3.0f * oneMinusT * t * t * _waypoints[index3] + t * t * t * _waypoints[index4];
}