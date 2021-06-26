#include "stdafx.h"
#include "Line3D.h"

#include "Geometry/General/BasicOperations.h"

/// [Public methods]

Line3D::Line3D()
{
}

Line3D::Line3D(const vec3& orig, const vec3& dest) :
	Edge3D(orig, dest)
{
}

Line3D::Line3D(const Line3D& line) :
	Edge3D(line)
{
}

Line3D::~Line3D()
{
}

float Line3D::distance(const vec3& point) const
{
	const vec3 ap = point - _orig;

	return glm::length(glm::cross(_direction, ap)) / glm::length(_direction);
}

float Line3D::distance(Line3D& line) const
{
	const vec3 line_u		= line._dest - line._orig;
	const vec3 cross		= glm::cross(_direction, line_u);
	const float dotLength	= glm::length(cross);

	if (BasicOperations::equal(dotLength, 0.0f))
	{
		return FLT_MAX;
	}

	const vec3 origDiff = _orig - line._orig;

	return std::abs(glm::dot(origDiff, cross)) / dotLength;
}

Line3D Line3D::normalToPoint(const vec3& p) const
{
	const float lambda	= glm::dot(_direction, p - _orig) / glm::dot(_direction, _direction);
	const vec3 u		= p - _orig + _direction * lambda;

	return Line3D(p, p + u);
}

Line3D& Line3D::operator=(const Line3D& line)
{
	Edge3D::operator=(line);

	return *this;
}

bool Line3D::parallel(Line3D& line) const
{
	return false;
}

bool Line3D::perpendicular(Line3D& line) const
{
	return BasicOperations::equal(glm::dot(_direction, line._direction), 0.0f);
}

/// [Protected methods]

bool Line3D::isParametricTValid(const float t) const
{
	return true;
}